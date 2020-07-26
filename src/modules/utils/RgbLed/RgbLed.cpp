#include "RgbLed.h"

/*
 * LED indicator:
 * off   = not paused, nothing to do
 * fast flash = halted
 * on    = a block is being executed
 */

#include "modules/robot/Conveyor.h"
#include "libs/Kernel.h"
#include "libs/StreamOutput.h"
#include "libs/StreamOutputPool.h"
#include "SlowTicker.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "Gcode.h"

#define rgb_led_enable_checksum   CHECKSUM("rgb_led_enable")
#define rgb_scl_pin_checksum       CHECKSUM("rgb_scl_pin")
#define rgb_sda_pin_checksum       CHECKSUM("rgb_sda_pin")
#define rgb_default_color          CHECKSUM("rgb_default_color")

RgbLed::RgbLed() {
    this->addr = 0x32 << 1;
}

void RgbLed::on_module_loaded()
{
    if(!THEKERNEL->config->value( rgb_led_enable_checksum )->by_default(true)->as_bool()) {
        delete this;
        return;
    }
    sda_pin.from_string(THEKERNEL->config->value( rgb_sda_pin_checksum )->by_default("0.27")->as_string()); // I2C SDA
    scl_pin.from_string(THEKERNEL->config->value( rgb_scl_pin_checksum  )->by_default("0.28")->as_string()); // I2C SCL

    sda_pin.as_open_drain()->pull_up();
    scl_pin.as_open_drain()->pull_up();

//    THEKERNEL->streams->printf("WARNING Watchdog is disabled\n");

      this->i2c = new mbed::I2C(P0_27, P0_28);
      this->i2c->frequency(20000);

      i2c_send(Color(0,127,0));
    THEKERNEL->slow_ticker->attach(12, this, &RgbLed::led_tick);
}

void RgbLed::i2c_send( const Color& c ){
    this->i2c->start();
    this->i2c->write(addr);
    this->i2c->write(0);
    for (int i = 0; i < 8; i++) {
        this->i2c->write(c.b);
        this->i2c->write(c.g);
        this->i2c->write(c.r);
        }
    this->i2c->stop();
}


uint32_t RgbLed::led_tick(uint32_t)
{
    static uint8_t cnt = 0, state = 0;

    sda_pin.set(cnt & 0);
    scl_pin.set(cnt & 0);

    if(THEKERNEL->is_halted()) {
        i2c_send(Color(255,1,1));
        return 0;
    } else if (!THECONVEYOR->is_idle()) {
        i2c_send(Color(1,255,1));
        return 0;
    } else {
        if(++cnt >= 6) { // 6 ticks ~ 500ms
            cnt = 0;
            state = !state;
            }
        if (state)
            i2c_send(Color(1,200,255));
        else
            i2c_send(Color(1,255,255));
    }

    return 0;
}




