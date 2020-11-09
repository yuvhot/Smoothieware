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
        rxtx(rxtxPin) {}

    bool read_bit() override {
        return true;
    }

    void send_bit(bool value) override {

    }

private:
    DigitalInOut rxtx;
};

#endif // HALFDUPLEXSERIALPINCONFIG_H
