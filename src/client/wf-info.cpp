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


#include <iostream>
#include <string.h>
#include <getopt.h>
#include <vector>

#include "wf-info.hpp"

static void registry_add(void *data, struct wl_registry *registry,
    uint32_t id, const char *interface,
    uint32_t version)
{
    WfInfo *wfm = (WfInfo *) data;

    if (strcmp(interface, wf_info_base_interface.name) == 0)
    {
        wfm->wf_information_manager = (wf_info_base *)
            wl_registry_bind(registry, id,
            &wf_info_base_interface, 1);
    }
}

static void registry_remove(void *data, struct wl_registry *registry,
    uint32_t id)
{
    exit(0);
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_add,
    .global_remove = registry_remove,
};

static void receive_view_info(void *data,
    struct wf_info_base *wf_info_base,
    const uint32_t view_id,
    const int client_pid,
    const int ws_x,
    const int ws_y,
    const char *app_id,
    const char *title,
    const char *role,
    const int x,
    const int y,
    const int width,
    const int height,
    const int xwayland,
    const int focused,
    const char * output_name,
    const uint32_t output_id)
{
    std::cout << "=========================" << std::endl;
    std::cout << "View ID: " << view_id << std::endl;
    std::cout << "Client PID: " << client_pid << std::endl;
    std::cout << "Output: " << output_name << "(ID: " << output_id << ")" << std::endl;
    std::cout << "Workspace: " << ws_x << "," << ws_y << std::endl;
    std::cout << "App ID: "  << app_id << std::endl;
    std::cout << "Title: " << title << std::endl;
    std::cout << "Role: " << role << std::endl;
    std::cout << "Geometry: " << x << "," << y << " " << width << "x" << height << std::endl;
    std::cout << "Xwayland: " << (xwayland ? "true" : "false") << std::endl;
    std::cout << "Focused: " << (focused ? "true" : "false") << std::endl;
    std::cout << "=========================" << std::endl;
}

static void done(void *data,
    struct wf_info_base *wf_info_base)
{
    exit(0);
}

static struct wf_info_base_listener information_base_listener {
	.view_info = receive_view_info,
	.done = done,
};

WfInfo::WfInfo(int argc, char *argv[])
{
    display = wl_display_connect(NULL);
    if (!display)
    {
        return;
    }

    wl_registry *registry = wl_display_get_registry(display);
    if (!registry)
    {
        return;
    }

    wl_registry_add_listener(registry, &registry_listener, this);

    wf_information_manager = NULL;
    wl_display_roundtrip(display);
    wl_registry_destroy(registry);
    if (!wf_information_manager)
    {
        std::cout << "Wayfire information protocol not advertised by compositor. Is wf-info plugin enabled?" << std::endl;
        return;
    }

    wf_info_base_add_listener(wf_information_manager,
        &information_base_listener, this);

    struct option opts[] = {
        { "view-id",     required_argument, NULL, 'i' },
        { "all-views",   no_argument,       NULL, 'l' },
        { 0,             0,                 NULL,  0  }
    };

    std::vector<int> view_ids;
    int c, i, list_all_views = 0;
    while((c = getopt_long(argc, argv, "i:l", opts, &i)) != -1)
    {
        switch(c)
        {
            case 'i':
                view_ids.push_back(atoi(optarg));
                break;

            case 'l':
                list_all_views = 1;
                break;

            default:
                printf("Unsupported command line argument %s\n", optarg);
        }
    }

    for (auto view_id : view_ids)
    {
        wf_info_base_view_info_id(wf_information_manager, view_id);
    }

    if (list_all_views)
    {
        wf_info_base_view_info_list(wf_information_manager);
    }
    else if (view_ids.empty())
    {
        wf_info_base_view_info(wf_information_manager);
    }

    while(1)
        wl_display_dispatch(display);

    wl_display_flush(display);
    wl_display_disconnect(display);
}

WfInfo::~WfInfo()
{
}

int main(int argc, char *argv[])
{
    WfInfo(argc, argv);

    return 0;
}
