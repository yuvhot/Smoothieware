#ifndef OPENLOOPPWMSPINDLECONTROL_H
#define OPENLOOPPWMSPINDLECONTROL_H

#include "SpindleControl.h"

namespace mbed {
    class PwmOut;
    class DigitalOut;
}

class OpenLoopPWMSpindleControl : public SpindleControl{
public:
    OpenLoopPWMSpindleControl();
    virtual ~OpenLoopPWMSpindleControl() {};
    void on_module_loaded() override;
private:
    void set_speed(int speed_rpm) override;
    void report_speed(void) override;
    void turn_on(bool forward) override;
    void turn_off() override;

    mbed::PwmOut *pwm_pin;
    mbed::DigitalOut *forward_pin;
    mbed::DigitalOut *reverse_pin;

    int current_speed_rpm;
    float current_pwm_value;
    bool spindle_reverse;
    int min_rpm;
    int max_rpm;
    int change_direction_dwell_ms;
    int spin_up_dwell_ms;
};


#endif // OPENLOOPPWMSPINDLECONTROL_H
