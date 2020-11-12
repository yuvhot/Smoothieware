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
        rxtx(rxtxPin) {
        rxtx.mode(PinMode::PullUp);
    }

    bool read_bit() override {
        return rxtx.read();
    }

    void send_bit(bool value, bool last) override {
        rxtx.mode((value || last) ? PinMode::PullUp : PinMode::PullDown);
    }

private:
    DigitalIn rxtx;
};

#endif // HALFDUPLEXSERIALPINCONFIG_H
