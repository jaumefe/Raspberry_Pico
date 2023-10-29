#include "led_matrix_i2c.h"

const RGBLookup_t lookup_table[28] = {
    {118, 69, 85},
    {117, 68, 101},
    {116, 84, 100},
    {115, 83, 99},
    {114, 82, 98},
    {113, 81, 97},
    {112, 80, 96},
    {134, 21, 37},
    {133, 20, 36},
    {132, 19, 35},
    {131, 18, 34},
    {130, 17, 50},
    {129, 33, 49},
    {128, 32, 48},
    {127, 47, 63},
    {121, 41, 57},
    {122, 25, 58},
    {123, 26, 42},
    {124, 27, 43},
    {125, 28, 44},
    {126, 29, 45},
    {15, 95, 111},
    {8, 89, 105},
    {9, 90, 106},
    {10, 91, 107},
    {11, 92, 108},
    {12, 76, 109},
    {13, 77, 93}
};

bool init() {
    bool success = IS31FL3731_init();
    uint8_t cmd[] = {
            0b00000000, 0b10111111,
            0b00111110, 0b00111110,
            0b00111111, 0b10111110,
            0b00000111, 0b10000110,
            0b00110000, 0b00110000,
            0b00111111, 0b10111110,
            0b00111111, 0b10111110,
            0b01111111, 0b11111110,
            0b01111111, 0b00000000
        };
    if (success) {
        IS31FL3731_enable(cmd, 0);
    }
    return success;
}

RGBLookup_t lookup_pixel(uint8_t index){
    return lookup_table[index];
}

void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b){
    if (x == 1 || x == 3) {
        y = 4 - y;
    }
    uint8_t index = y + (x * 5);
    RGBLookup_t rgb = lookup_pixel(index);
    IS31FL3731_set(rgb.r, r);
    IS31FL3731_set(rgb.g, g);
    IS31FL3731_set(rgb.b, b);
}