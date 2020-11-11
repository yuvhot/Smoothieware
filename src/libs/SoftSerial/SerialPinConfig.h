//
// Created by mkotyk on 11/8/20.
//

#ifndef SERIALPINCONFIG_H
#define SERIALPINCONFIG_H

#include "SerialParams.h"
#include "SerialBitReader.h"
#include "SerialBitWriter.h"
#include <functional>

class SerialPinConfig {
public:
    SerialPinConfig(std::function<void(const unsigned char)> emit,
                    std::function<void()> empty) :
            serial_bit_reader(params, emit),
            serial_bit_writer(params, empty) {}

    void tick(int phase) {
        // send_bit(phase == 1, phase == 0); // Test pattern
        serial_bit_writer.next([this](bool value, bool last) { send_bit(value, last); }, phase);
        serial_bit_reader.next(read_bit(), phase);
    }

    void setFormat(int bits = 8, SerialParams::Parity parity = SerialParams::Parity::None, int stop_bits = 1) {
        params.bits = bits;
        params.parity = parity;
        params.stop_bits = stop_bits;
    }

    bool writable() { return serial_bit_writer.writable(); }

    int putc(int data) { return serial_bit_writer.putc(data); }

private:
    SerialParams params;
    SerialBitReader serial_bit_reader;
    SerialBitWriter serial_bit_writer;

protected:
    virtual bool read_bit() = 0;

    virtual void send_bit(bool value, bool last) = 0;
};

#endif // SERIALPINCONFIG_H
