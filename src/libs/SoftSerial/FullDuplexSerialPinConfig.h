//
// Created by mkotyk on 11/9/20.
//

#ifndef FULLDUPLEXSERIALPINCONFIG_H
#define FULLDUPLEXSERIALPINCONFIG_H

#include "SerialPinConfig.h"
#include "mbed.h"

class FullDuplexSerialPinConfig : public SerialPinConfig {
public:
    FullDuplexSerialPinConfig(
            PinName rxPin,
            PinName txPin,
            std::function<void(const unsigned char)> emit,
            std::function<void()> empty
    ) : SerialPinConfig(emit, empty),
        rx(rxPin),
        tx(txPin) {
        rx.mode(PinMode::PullUp);
        tx.write(false);
    }

    bool read_bit() override {
        return rx.read();
    }

    void send_bit(bool value, bool last) override {
        tx.write(value);
    }

private:
    DigitalIn rx;
    DigitalOut tx;
};

#endif // FULLDUPLEXSERIALPINCONFIG_H
