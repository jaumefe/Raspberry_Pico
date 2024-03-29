#include <stdio.h>
#include "pico/stdlib.h"

int power (int base, int exponent){
    int res = 1;
    for(int i = 0; i < exponent; i++){
        res *= base;
    }
    return res;
}

int blinkLED(int state, const uint pin){
    uint newState = (state == 1) ? 0 : 1;
    gpio_put(pin, newState);
    return newState;
}