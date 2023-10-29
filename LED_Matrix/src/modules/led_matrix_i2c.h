#pragma once
#ifndef _LED_MATRIX_I2C_H_
#define _LED_MATRIX_I2C_H_

#include "is31fl3731.h"

struct RGBLookup_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

typedef struct RGBLookup_s RGBLookup_t;

struct RGBMatrix_s {
    uint8_t WIDTH;
    uint8_t HEIGHT;
};

bool init();
RGBLookup_t lookup_pixel(uint8_t index);
void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);

#endif