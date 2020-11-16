#include "OpenLoopPWMSpindleControl.h"
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "StreamOutputPool.h"
#include "SlowTicker.h"
#include "libs/Pin.h"
#include "PwmOut.h"


#define spindle_pwm_pin_checksum            CHECKSUM("pwm_pin")
#define spindle_forward_pin_checksum        CHECKSUM("forward_pin")
#define spindle_reverse_pin_checksum        CHECKSUM("reverse_pin")
#define spindle_pwm_period_checksum         CHECKSUM("pwm_period")
#define spindle_max_pwm_checksum            CHECKSUM("max_pwm")
#define spindle_default_rpm_checksum        CHECKSUM("default_rpm")
#define spindle_min_rpm_checksum            CHECKSUM("min_rpm")
#define spindle_max_rpm_checksum            CHECKSUM("max_rpm")

OpenLoopPWMSpindleControl::OpenLoopPWMSpindleControl() {

}

void OpenLoopPWMSpindleControl::on_module_loaded() {

}

void OpenLoopPWMSpindleControl::turn_on(bool forward) {
    spindle_on = true;
    spindle_reverse = !forward;
}

void OpenLoopPWMSpindleControl::turn_off() {
    spindle_on = false;
}


void OpenLoopPWMSpindleControl::set_speed(int rpm) {
    current_speed_rpm = rpm;
}


void OpenLoopPWMSpindleControl::report_speed() {
    THEKERNEL->streams->printf("Current RPM: %5d  PWM value: %5.3f\n", current_speed_rpm, current_pwm_value);
}
