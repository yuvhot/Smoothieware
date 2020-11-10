//
// Created by mkotyk on 11/8/20.
//

#ifndef SERIALBITWRITER_H
#define SERIALBITWRITER_H

#include "SerialParams.h"
#include <functional>

class SerialBitWriter {
private:
    SerialParams *_params;
    int _total_bits;
    int send_buffer, tx_bit;
    std::function<void()> _empty;
public:
    SerialBitWriter(SerialParams *params,
                    std::function<void()> empty) :
            _params(params),
            _total_bits(0),
            send_buffer(0),
            tx_bit(-1),
            _empty(empty) {
    }

    bool writable() { return tx_bit == -1; }

    int putc(unsigned char data) {
        if (tx_bit != -1) return -1;

        send_buffer = data << 1;

        bool parity;
        switch (_params->parity) {
            case SerialParams::Parity::Forced1:
                send_buffer |= 1 << (_params->bits + 1);
                break;
            case SerialParams::Parity::Forced0:
                send_buffer &= ~(1 << (_params->bits + 1));
                break;
            case SerialParams::Parity::Even:
                parity = false;
                for (int i = 0; i < _params->bits; i++) {
                    if (((send_buffer >> i) & 0x01) == 1)
                        parity = !parity;
                }
                send_buffer |= parity << (_params->bits + 1);
                break;
            case SerialParams::Parity::Odd:
                parity = true;
                for (int i = 0; i < _params->bits; i++) {
                    if (((send_buffer >> i) & 0x01) == 1)
                        parity = !parity;
                }
                send_buffer |= parity << (_params->bits + 1);
                break;
            case SerialParams::Parity::None:
                // No parity, nothing to do here
                break;
        }

        send_buffer |= 0xFFFF << (1 + _params->bits + (bool) _params->parity);
        send_buffer &= ~(1 << _total_bits);
        tx_bit = 0;
        return 1;
    }

    void next(std::function<void(bool, bool)> send, int phase) {
        if (tx_bit >= 0 && phase == 0) {
            send((send_buffer >> tx_bit) & 1, tx_bit == 11);
            if (tx_bit == 11) {
                tx_bit = -1;
            } else {
                tx_bit++;
            }
        }
    }
};

#endif // SERIALBITWRITER_H
