#include "OpenLoopPWMSpindleControl.h"
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "libs/Config.h"
#include "libs/ConfigValue.h"
#include "libs/checksumm.h"
#include "libs/StreamOutputPool.h"
#include "libs/SlowTicker.h"
#include "libs/Pin.h"
#include "libs/utils.h"
#include "PwmOut.h"

#define spindle_checksum                            CHECKSUM("spindle")
#define spindle_pwm_pin_checksum                    CHECKSUM("pwm_pin")
#define spindle_forward_pin_checksum                CHECKSUM("forward_pin")
#define spindle_reverse_pin_checksum                CHECKSUM("reverse_pin")
#define spindle_pwm_period_checksum                 CHECKSUM("pwm_period")
#define spindle_max_pwm_checksum                    CHECKSUM("max_pwm")
#define spindle_default_rpm_checksum                CHECKSUM("default_rpm")
#define spindle_min_rpm_checksum                    CHECKSUM("min_rpm")
#define spindle_max_rpm_checksum                    CHECKSUM("max_rpm")
#define spindle_change_direction_dwell_checksum     CHECKSUM("change_direction_dwell_ms")

#define MIN(a, b) ((a<b)?a:b)
#define MAX(a, b) ((a>b)?a:b)

OpenLoopPWMSpindleControl::OpenLoopPWMSpindleControl() {}

OpenLoopPWMSpindleControl::~OpenLoopPWMSpindleControl() {
}

void OpenLoopPWMSpindleControl::on_module_loaded() {
    spindle_on = false;
    spindle_reverse = false;

    // Get the pin for hardware pwm
    {
        Pin *pin = new Pin();
        pin->from_string(THEKERNEL->config
                                 ->value(spindle_checksum, spindle_pwm_pin_checksum)
                                 ->by_default("nc")
                                 ->as_string());
        pwm_pin = pin->as_output()->hardware_pwm();
        pwm_output_inverted = pin->is_inverting();
        delete pin;
    }

    if (pwm_pin == NULL) {
        THEKERNEL->streams->printf("Error: Spindle PWM pin must be P2.0-2.5 or other PWM pin\n");
        delete this;
        return;
    }

    forward_pin.from_string(THEKERNEL->config
                                    ->value(spindle_checksum, spindle_forward_pin_checksum)
                                    ->by_default("nc")
                                    ->as_string()
    )->as_output();
    if (!forward_pin.connected()) {
        THEKERNEL->streams->printf("Error: Spindle forward pin must be defined\n");
        delete this;
        return;
    }

    // This could be optional, but for now I'm making it required
    reverse_pin.from_string(THEKERNEL->config
                                    ->value(spindle_checksum, spindle_reverse_pin_checksum)
                                    ->by_default("nc")
                                    ->as_string()
    )->as_output();
    if (!reverse_pin.connected()) {
        THEKERNEL->streams->printf("Error: Spindle reverse pin must be defined\n");
        delete this;
        return;
    }

    turn_off();

    min_rpm = THEKERNEL->config
            ->value(spindle_checksum, spindle_min_rpm_checksum)
            ->by_default(0)
            ->as_int();
    max_rpm = THEKERNEL->config
            ->value(spindle_checksum, spindle_max_rpm_checksum)
            ->by_default(0)
            ->as_int();
    if (max_rpm - min_rpm <= 0) {
        THEKERNEL->streams->printf("Error: Spindle rpm range is invalid (%d-%d)\n", min_rpm, max_rpm);
        delete this;
        return;
    }
    default_rpm = THEKERNEL->config
            ->value(spindle_checksum, spindle_default_rpm_checksum)
            ->by_default(0)
            ->as_int();
    change_direction_dwell_ms = THEKERNEL->config
            ->value(spindle_checksum, spindle_change_direction_dwell_checksum)
            ->by_default(250)
            ->as_int();
    max_pwm = THEKERNEL->config
            ->value(spindle_checksum, spindle_max_pwm_checksum)
            ->by_default(1.0f)
            ->as_number();
    int period = THEKERNEL->config
            ->value(spindle_checksum, spindle_pwm_period_checksum)
            ->by_default(1000)
            ->as_int();
    pwm_pin->period_us(period);
    pwm_pin->write(pwm_output_inverted ? 1 : 0);
    set_speed(default_rpm);
}

void OpenLoopPWMSpindleControl::turn_on(bool forward) {
    // Changing direction
    if (spindle_on && spindle_reverse != forward) {
        forward_pin.set(false);
        reverse_pin.set(false);
        safe_delay_ms(change_direction_dwell_ms);
    }

    forward_pin.set(forward);
    reverse_pin.set(!forward);

    spindle_on = true;
    spindle_reverse = !forward;

}

void OpenLoopPWMSpindleControl::turn_off() {
    forward_pin.set(false);
    reverse_pin.set(false);
    spindle_on = false;
}

void OpenLoopPWMSpindleControl::set_speed(int rpm) {
    SpindleControl::set_speed(rpm);
    current_speed_rpm = rpm;
    current_pwm_value = calc_pwm_from_rpm(rpm);

    if (pwm_output_inverted) {
        pwm_pin->write(1.0f - current_pwm_value);
    } else {
        pwm_pin->write(current_pwm_value);
    }
}

float OpenLoopPWMSpindleControl::calc_pwm_from_rpm(int rpm) const {
    return (float) (MAX(MIN(rpm, max_rpm), min_rpm)) / (float) (max_rpm - min_rpm);
}

void OpenLoopPWMSpindleControl::report_speed() {
    THEKERNEL->streams->printf("Current RPM: %5d  PWM value: %5.3f\n", current_speed_rpm, current_pwm_value);
}
