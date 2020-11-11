//
// Created by mkotyk on 11/9/20.
//

#ifndef HALFDUPLEXSERIALPINCONFIG_H
#define HALFDUPLEXSERIALPINCONFIG_H

#include "SerialPinConfig.h"
#include "mbed.h"

class HalfDuplexSerialPinConfig : public SerialPinConfig {
public:
    HalfDuplexSerialPinConfig(
            PinName rxtxPin,
            std::function<void(const unsigned char)> emit,
            std::function<void()> empty
    ) : SerialPinConfig(emit, empty),
        rxtx(rxtxPin),
        sending(false) {
        rxtx.write(true);
        rxtx.input();
        rxtx.mode(PinMode::PullUp);
    }

    bool read_bit() override {
        if (sending) {
            return false;
        }
        return rxtx.read();
    }

    void send_bit(bool value, bool last) override {
        if (!sending) {
            rxtx.output();
            sending = true;
        }

        if (last) {
            rxtx.write(true);
            rxtx.mode(PinMode::PullUp);
            rxtx.input();
            sending = false;
        } else {
            rxtx.write(value);
        }
    }

private:
    DigitalInOut rxtx;
    bool sending;
};

#endif // HALFDUPLEXSERIALPINCONFIG_H
