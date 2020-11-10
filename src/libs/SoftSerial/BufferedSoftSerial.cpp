/**
 * @file    BufferedSoftSerial.cpp
 * @brief   Software Buffer - Extends mbed Serial functionallity adding irq driven TX and RX
 * @author  sam grove
 * @version 1.0
 * @see
 *
 * Copyright (c) 2013
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "BufferedSoftSerial.h"
#include <stdarg.h>
#include "PolledSoftSerialHandler.h"
#include "FullDuplexSerialPinConfig.h"
#include "HalfDuplexSerialPinConfig.h"

BufferedSoftSerial::BufferedSoftSerial(PinName tx, PinName rx, const char *name) {
    if (rx == tx) {
        // Half duplex mode
        serialPinConfig = new HalfDuplexSerialPinConfig(
                rx,
                [this](const unsigned char c) { this->byteIn(c); },
                [this]() { this->prime(); }
        );

    } else {
        serialPinConfig = new FullDuplexSerialPinConfig(
                rx,
                tx,
                [this](const unsigned char c) { this->byteIn(c); },
                [this]() { this->prime(); }
        );
    }
    PolledSoftSerialHandler::instance().addPinConfig(serialPinConfig);
}


void BufferedSoftSerial::baud(int baudrate) {
    PolledSoftSerialHandler::instance().setBaud(baudrate);
}

void BufferedSoftSerial::format(int bits, SerialParams::Parity parity, int stop_bits) {
    serialPinConfig->setFormat(bits, parity, stop_bits);
}

int BufferedSoftSerial::readable(void) {
    return _rxbuf.size();  // note: look if things are in the buffer
}

int BufferedSoftSerial::writeable(void) {
    return 1;   // buffer allows overwriting by design, always true
}

int BufferedSoftSerial::getc(void) {
    char retval;
    _rxbuf.pop_front(retval);
    return (int) retval;
}

int BufferedSoftSerial::putc(int c) {
    _txbuf.push_back((char) c);
    BufferedSoftSerial::prime();

    return c;
}

int BufferedSoftSerial::puts(const char *s) {
    const char *ptr = s;

    while (*(ptr) != 0) {
        _txbuf.push_back(*(ptr++));
    }
    _txbuf.push_back('\n');  // done per puts definition
    BufferedSoftSerial::prime();

    return (ptr - s) + 1;
}

int BufferedSoftSerial::printf(const char *format, ...) {
    char buf[256] = {0};
    int r = 0;

    va_list arg;
    va_start(arg, format);
    r = vsprintf(buf, format, arg);
    va_end(arg);
    r = BufferedSoftSerial::write(buf, r);

    return r;
}

ssize_t BufferedSoftSerial::write(const void *s, size_t length) {
    const char *ptr = (const char *) s;
    const char *end = ptr + length;

    while (ptr != end) {
        _txbuf.push_back(*(ptr++));
    }
    BufferedSoftSerial::prime();

    return ptr - (const char *) s;
}

void BufferedSoftSerial::prime() {
    if (serialPinConfig->writable()) {
        if (_txbuf.size()) {
            char rv;
            _txbuf.pop_front(rv);
            serialPinConfig->putc(rv);
        }
    }
}

void BufferedSoftSerial::byteIn(unsigned char c) {
    _rxbuf.push_back(c);
    if (_emit) _emit(c);
}