#ifndef _RGBLED_H
#define _RGBLED_H

#include "libs/Kernel.h"
#include "I2C.h" // mbed.h lib
#include "libs/Pin.h"

class Color {
public:
    Color() : r(0), g(0), b(0) {};
    Color(int _r, int _g, int _b) : r(_r), g(_g), b(_b) {};

    uint8_t r,g,b;
};


class RgbLed : public Module
{
public:
    RgbLed();

    void on_module_loaded(void);
    void on_config_reload(void *);

private:
    void i2c_send(const Color& c);
    uint32_t led_tick(uint32_t);

    uint8_t addr;
    mbed::I2C* i2c;

    Pin sda_pin, scl_pin;

};

#endif /* _RGBLED_H */
