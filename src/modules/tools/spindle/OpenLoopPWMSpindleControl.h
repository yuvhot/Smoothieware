#ifndef OPENLOOPPWMSPINDLECONTROL_H
#define OPENLOOPPWMSPINDLECONTROL_H

#include "SpindleControl.h"
#include "libs/Pin.h"

namespace mbed {
    class PwmOut;
}

class OpenLoopPWMSpindleControl : public SpindleControl{
public:
    OpenLoopPWMSpindleControl();
    virtual ~OpenLoopPWMSpindleControl();
    void on_module_loaded() override;
private:
    void set_speed(int speed_rpm) override;
    void report_speed(void) override;
    void turn_on(bool forward) override;
    void turn_off() override;
    float calc_pwm_from_rpm(int rpm) const;

    mbed::PwmOut *pwm_pin;
    Pin forward_pin;
    Pin reverse_pin;

    int current_speed_rpm;
    float current_pwm_value;
    float max_pwm;
    bool pwm_output_inverted;
    bool spindle_reverse;
    int min_rpm;
    int max_rpm;
    int default_rpm;
    int change_direction_dwell_ms;
};


#endif // OPENLOOPPWMSPINDLECONTROL_H
