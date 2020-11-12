//
// Created by mkotyk on 11/8/20.
//

#ifndef SERIALBITWRITER_H
#define SERIALBITWRITER_H

#include "SerialParams.h"
#include <functional>

class SerialBitWriter {
private:
    const SerialParams& _params;
    int _send_buffer, _tx_bit, _total_bits;
    std::function<void()> _empty;
public:
    SerialBitWriter(const SerialParams& params,
                    std::function<void()> empty) :
            _params(params),
            _send_buffer(0),
            _tx_bit(100),
            _total_bits(0),
            _empty(empty) {
    }

    bool writable() { return _tx_bit > _total_bits; }

    int putc(unsigned char data) {
        if (!writable()) return -1;
        const int start_bits = 1;
        _send_buffer = data << start_bits;
        bool parity = false;
        int parity_bits = 0;
        switch (_params.parity) {
            case SerialParams::Parity::Forced1:
                _send_buffer |= 1 << (_params.bits + start_bits);
                parity_bits = 1;
                break;
            case SerialParams::Parity::Forced0:
                _send_buffer &= ~(1 << (_params.bits + start_bits));
                parity_bits = 1;
                break;
            case SerialParams::Parity::Even:
                for (int i = 0; i < _params.bits; i++) {
                    if (((_send_buffer >> i) & 0x01) == 1)
                        parity = !parity;
                }
                _send_buffer |= parity << (_params.bits + start_bits);
                parity_bits = 1;
                break;
            case SerialParams::Parity::Odd:
                parity = true;
                for (int i = 0; i < _params.bits; i++) {
                    if (((_send_buffer >> i) & 0x01) == 1)
                        parity = !parity;
                }
                _send_buffer |= parity << (_params.bits + start_bits);
                parity_bits = 1;
                break;
            case SerialParams::Parity::None:
                // No parity, nothing to do here
                break;
        }
        _total_bits = _params.bits + _params.stop_bits + parity_bits + start_bits;
        _send_buffer |= 0xFFFF << (start_bits + _params.bits + parity_bits); // Add stop bits as needed
        _send_buffer &= ~(1 << _total_bits);
        // printf("TX: %02X %04X:%d\n", data, _send_buffer & 0xFFFF, _total_bits); // Debugging
        _tx_bit = 0;
        return 1;
    }

    void next(std::function<void(bool, bool)> send, int phase) {
        if (_tx_bit <= _total_bits && phase == 0) {
            send((_send_buffer >> _tx_bit) & 1, _tx_bit == _total_bits);
            _tx_bit++;
            if (writable()) _empty();
        }
    }
};

#endif // SERIALBITWRITER_H
