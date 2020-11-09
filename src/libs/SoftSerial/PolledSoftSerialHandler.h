//
// Created by mkotyk on 11/8/20.
//

#ifndef POLLEDSOFTSERIALHANDLER_H
#define POLLEDSOFTSERIALHANDLER_H

#include <stdint.h>
#include <vector>

class SerialPinConfig;

class PolledSoftSerialHandler {
public:
    /**
     * Set the desired baud rate.
     * NOTE! This changes the baud rate for all configured pins.
     *
     * @param baud The baud rate, ie 19200
     */
    void setBaud(int baud);

    void addPinConfig(SerialPinConfig *serial_pin_config) { pin_configs.push_back(serial_pin_config); }

    void removePinConfig(SerialPinConfig *serial_pin_config) { pin_configs.push_back(serial_pin_config); }

    /**
     * Singleton instance, so that only one object controls Repeating Interrupt Timer
     * @return The instance
     */
    static PolledSoftSerialHandler &instance() {
        static PolledSoftSerialHandler instance;
        return instance;
    }

    void handle_interrupt();

private:
    int baud;
    int tickCount;

    PolledSoftSerialHandler();

    std::vector<SerialPinConfig *> pin_configs;
};

#endif // POLLEDSOFTSERIALHANDLER_H
