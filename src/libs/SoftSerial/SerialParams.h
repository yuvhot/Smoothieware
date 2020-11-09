//
// Created by mkotyk on 11/9/20.
//

#ifndef SERIALPARAMS_H
#define SERIALPARAMS_H

class SerialParams {
public:
    enum Parity {
        None = 0,
        Odd,
        Even,
        Forced1,
        Forced0
    };

    int bits;
    Parity parity;
    int stop_bits;
};

#endif // SERIALPARAMS_H
