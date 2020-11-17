/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#include "libs/Module.h"
#include "libs/Kernel.h"
#include "Robot.h"
#include "Gcode.h"
#include "Conveyor.h"
#include "SpindleControl.h"
#include "checksumm.h"
#include "PublicDataRequest.h"
#include "SwitchPublicAccess.h"
#include "utils.h"

#define spindle_on_checksum            CHECKSUM("spindle_on")
#define spindle_fwd_checksum           CHECKSUM("spindle_fwd")

void SpindleControl::on_get_public_data(void *argument)
{
    PublicDataRequest* pdr = (PublicDataRequest *) argument;
    if (pdr->starts_with(switch_checksum)) {
        if(pdr->second_element_is(spindle_on_checksum)) {
            ((struct pad_switch *) pdr->get_data_ptr())->state = spindle_on;
            pdr->set_taken();
        }
        if(pdr->second_element_is(spindle_fwd_checksum)) {
            ((struct pad_switch *) pdr->get_data_ptr())->state = !spindle_reverse;
            pdr->set_taken();
        }
    }
}

void SpindleControl::on_gcode_received(void *argument)
{

    Gcode *gcode = static_cast<Gcode *>(argument);

    if(gcode->has_letter('S')) {
        // S-Words can appear on their own or before an M[3-5] code
        set_speed(gcode->get_value('S'));
    }

    if (gcode->has_m) {
        if (gcode->m == 957)
        {
            // M957: report spindle speed
            report_speed();
        }
        else if (gcode->m == 958)
        {
            THECONVEYOR->wait_for_idle();
            // M958: set spindle PID parameters
            if (gcode->has_letter('P'))
                set_p_term( gcode->get_value('P') );
            if (gcode->has_letter('I'))
                set_i_term( gcode->get_value('I') );
            if (gcode->has_letter('D'))
                set_d_term( gcode->get_value('D') );
            // report PID settings
            report_settings();

        }
        else if (gcode->m == 3 || gcode->m == 4)
        {
            THECONVEYOR->wait_for_idle();
            // M3 with S value provided: set speed
            if (gcode->has_letter('S'))
            {
                set_speed(gcode->get_value('S'));
            }

            // M3: Spindle on
            turn_on(gcode->m == 3);
        }
        else if (gcode->m == 5)
        {
            THECONVEYOR->wait_for_idle();
            // M5: spindle off
            turn_off();
        }
    }
}

void SpindleControl::set_speed(int speed) {
    THEROBOT->set_s_value((float) speed);
}

void SpindleControl::on_halt(void *argument)
{
    if (argument == nullptr) {
        if(spindle_on) {
            // This is dangerous as turn_off calls on_delay in some instances
            turn_off();
        }
    }
}
