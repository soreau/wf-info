/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Moreau
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


#include <wayfire/plugin.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/util/log.hpp>

#include "plugin/wayfire-information.hpp"

class wf_info : public wf::plugin_interface_t, public wf::pointer_interaction_t
{
  public:
    std::unique_ptr<wayfire_information> wayfire_information_ptr = nullptr;

    void init()
    {
        wayfire_information_ptr = std::make_unique<wayfire_information>();
        wayfire_information_ptr->set_base_ptr(this);
    }

    void handle_pointer_button(const wlr_pointer_button_event& event) override
    {
        if (event.state == WL_POINTER_BUTTON_STATE_PRESSED)
        {
            wayfire_information_ptr->end_grab();
        }
    }

    void fini()
    {
        wayfire_information_ptr.reset();
    }
};

DECLARE_WAYFIRE_PLUGIN(wf_info);
