/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Moreau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <sys/time.h>
#include <wayfire/core.hpp>
#include <wayfire/view.hpp>
#include <wayfire/plugin.hpp>
#include <wayfire/output.hpp>
#include <wayfire/output-layout.hpp>
#include <wayfire/workspace-manager.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/nonstd/wlroots-full.hpp>
#include <linux/input-event-codes.h>
#include <wayfire/util/log.hpp>

#include "wayfire-information.hpp"
#include "wayfire-information-server-protocol.h"

extern "C"
{
#include <wlr/types/wlr_seat.h>
}

static void bind_manager(wl_client *client, void *data,
    uint32_t version, uint32_t id);

void wayfire_information::send_view_info(wayfire_view view)
{
    if (!view)
    {
        return;
    }
    auto output = view->get_output();
    if (!output)
    {
        return;
    }

    std::string role;
    switch (view->role)
    {
        case wf::VIEW_ROLE_TOPLEVEL:
            role = "TOPLEVEL";
            break;
        case wf::VIEW_ROLE_UNMANAGED:
            role = "UNMANAGED";
            break;
        case wf::VIEW_ROLE_DESKTOP_ENVIRONMENT:
            role = "DESKTOP_ENVIRONMENT";
            break;
        default:
            role = "UNKNOWN";
            break;
    }

    auto og = output->get_screen_size();
    auto ws = output->workspace->get_current_workspace();
    auto wm = view->transform_region(view->get_wm_geometry());
    wf::point_t workspace = {
        ws.x + (int)std::floor((wm.x + wm.width / 2.0) / og.width),
        ws.y + (int)std::floor((wm.y + wm.height / 2.0) / og.height)
    };

    auto vg = view->get_wm_geometry();

    pid_t pid = 0;
    wlr_surface *wlr_surface = view->get_wlr_surface();
    int is_xwayland_surface = 0;
#if WF_HAS_XWAYLAND
    is_xwayland_surface = wlr_surface_is_xwayland_surface(wlr_surface);
    if (is_xwayland_surface)
    {
        pid = wlr_xwayland_surface_from_wlr_surface(wlr_surface)->pid;
    } else
#endif
    {
        wl_client_get_credentials(view->get_client(), &pid, 0, 0);
    }

    int focused = wf::get_core().get_active_output()->get_active_view() == view;

    for (auto r : client_resources)
    {
        wf_info_base_send_view_info(r, view->get_id(),
                                       pid,
                                       workspace.x,
                                       workspace.y,
                                       view->get_app_id().c_str(),
                                       view->get_title().c_str(),
                                       role.c_str(),
                                       vg.x,
                                       vg.y,
                                       vg.width,
                                       vg.height,
                                       is_xwayland_surface,
                                       focused);
    }
}

void wayfire_information::deactivate()
{

    for (auto& o : wf::get_core().output_layout->get_outputs())
    {
	o->deactivate_plugin(grab_interfaces[o]);
        grab_interfaces[o]->ungrab();
    }

    idle_set_cursor.run_once([this] ()
    {
        wf::get_core().set_cursor("default");
        send_view_info(wf::get_core().get_cursor_focus_view());
        for (auto r : client_resources)
        {
            wf_info_base_send_done(r);
        }
    });
}

wayfire_information::wayfire_information()
{
    manager = wl_global_create(wf::get_core().display,
        &wf_info_base_interface, 1, this, bind_manager);

    if (!manager)
    {
        LOGE("Failed to create wayfire_information interface");
        return;
    }

    for (auto& o : wf::get_core().output_layout->get_outputs())
    {
        grab_interfaces[o] = std::make_unique<wf::plugin_grab_interface_t> (o);
        grab_interfaces[o]->name = "wf-info";
        grab_interfaces[o]->capabilities = wf::CAPABILITY_GRAB_INPUT;
    }
}

wayfire_information::~wayfire_information()
{
    wl_global_destroy(manager);
}

static void get_view_info(struct wl_client *client, struct wl_resource *resource)
{
    wayfire_information *wd = (wayfire_information*)wl_resource_get_user_data(resource);

    for (auto& o : wf::get_core().output_layout->get_outputs())
    {
        if (!o->activate_plugin(wd->grab_interfaces[o]))
        {
            continue;
        }

        wd->grab_interfaces[o]->callbacks.pointer.button = [=] (uint32_t b, uint32_t s)
        {
            if (s == WL_POINTER_BUTTON_STATE_PRESSED)
            {
                wd->deactivate();
            }
        };

        wd->grab_interfaces[o]->grab();
    }

    wd->idle_set_cursor.run_once([wd] ()
    {
        wf::get_core().set_cursor("crosshair");
    });
}

static void send_all_views(struct wl_client *client, struct wl_resource *resource)
{
    wayfire_information *wd = (wayfire_information*)wl_resource_get_user_data(resource);

    for (auto& output : wf::get_core().output_layout->get_outputs())
    {
        for (auto& view : output->workspace->get_views_in_layer(wf::ALL_LAYERS))
        {
            if (view->role != wf::VIEW_ROLE_TOPLEVEL &&
                view->role != wf::VIEW_ROLE_DESKTOP_ENVIRONMENT)
            {
                continue;
            }
            wd->send_view_info(view);
        }
    }

    for (auto r : wd->client_resources)
    {
        wf_info_base_send_done(r);
    }
}

static const struct wf_info_base_interface wayfire_information_impl =
{
    .view_info      = get_view_info,
    .view_info_list = send_all_views,
};

static void destroy_client(wl_resource *resource)
{
    wayfire_information *wd = (wayfire_information*)wl_resource_get_user_data(resource);

    for (auto& r : wd->client_resources)
    {
        if (r == resource)
        {
            r = nullptr;
        }
    }
    wd->client_resources.erase(std::remove(wd->client_resources.begin(),
        wd->client_resources.end(), nullptr), wd->client_resources.end());
}

static void bind_manager(wl_client *client, void *data,
    uint32_t version, uint32_t id)
{
    wayfire_information *wd = (wayfire_information*)data;

    auto resource =
        wl_resource_create(client, &wf_info_base_interface, 1, id);
    wl_resource_set_implementation(resource,
        &wayfire_information_impl, data, destroy_client);
    wd->client_resources.push_back(resource);
    
}
