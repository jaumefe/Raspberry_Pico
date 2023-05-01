#include <stdio.h>
#include "pico/stdlib.h"

#ifndef BASIC_H
#define BASIC_H
    int power (int base, int exponent);
    void blinkLED(int state, const uint pin);
#endif //BASIC_H