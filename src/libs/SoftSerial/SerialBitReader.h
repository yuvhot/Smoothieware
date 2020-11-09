//
// Created by mkotyk on 11/8/20.
//

#ifndef SERIALBITREADER_H
#define SERIALBITREADER_H

#include "SerialParams.h"
#include <functional>

class SerialBitReader {
private:
    SerialParams *_params;
    int error_count;
    bool rx_error = false;
    int read_buffer, rx_bit;
    int phase_sync;
    std::function<void(const unsigned char)> _emit;
public:
    SerialBitReader(SerialParams *params,
                    std::function<void(const unsigned char)> emit) :
            _params(params),
            error_count(0),
            rx_error(false),
            read_buffer(0),
            rx_bit(-1),
            phase_sync(0),
            _emit(emit) {
    }

    void next(bool val, int phase) {
        // Wait for start bit
        if (rx_bit < 0) {
            if (val) {
                phase_sync = phase;
                rx_bit = 0;
            }
            return;
        }

        // Wait for correct phase
        if (phase != phase_sync) {
            return;
        }

        // Shift bits in
        if (rx_bit < _params->bits) {
            read_buffer |= val << rx_bit;
            rx_bit++;
            return;
        }

        // Receive parity
        bool parity_count;
        if (rx_bit == _params->bits) {
            rx_bit++;
            switch (_params->parity) {
                case SerialParams::Parity::Forced1:
                    if (val == 0)
                        rx_error = true;
                    return;
                case SerialParams::Parity::Forced0:
                    if (val == 1)
                        rx_error = true;
                    return;
                case SerialParams::Parity::Even:
                case SerialParams::Parity::Odd:
                    parity_count = val;
                    for (int i = 0; i < _params->bits; i++) {
                        if (((read_buffer >> i) & 0x01) == 1)
                            parity_count = !parity_count;
                    }
                    if ((parity_count) && (_params->parity == SerialParams::Parity::Even))
                        rx_error = true;
                    if ((!parity_count) && (_params->parity == SerialParams::Parity::Odd))
                        rx_error = true;
                    return;
                case SerialParams::Parity::None:
                    // No parity, nothing to do here
                    break;
            }
        }

        // Receive stop
        if (rx_bit < _params->bits + (bool) _params->parity + _params->stop_bits) {
            rx_bit++;
            if (!val)
                rx_error = true;
            return;
        }

        // The last stop bit
        if (!val)
            rx_error = true;

        if (!rx_error && _emit) {
            _emit(read_buffer & 0xFF);
        } else {
            error_count++;
        }
        read_buffer = 0;
        rx_bit = -1;
    }
};

#endif // SERIALBITREADER_H
