#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

#ifndef _IS31FL3731_H_
#define _IS31FL3731_H_

#define DEFAULT_I2C_ADDRES      0x74
#define CONFIG_BANK             0x0B
#define NUM_PIXELS              144
#define NUM_FRAMES              8

#define ENABLE_OFFSET           0x00
#define COLOR_OFFSET            0x24

#define PICTURE                 0x00
#define AUTOPLAY                0x08
#define AUDIOPLAY               0x18

#define MODE                    0x00
#define FRAME                   0x01
#define AUTPLAY1                0x02
#define AUTPLAY2                0x03
#define BLINK                   0x05
#define AUDIOSYNC               0x06
#define BREATH1                 0x07
#define BREATH2                 0x08
#define SHUTDOWN                0x0a
#define GAIN                    0x0b
#define ADC                     0x0c
#define BANK                    0xfd

bool IS31FL3731_init();
void IS31FL3731_clear();
void IS31FL3731_update(uint8_t frame);
void IS31FL3731_enable(uint8_t * list, uint8_t frame);
void IS31FL3731_set(uint8_t index, uint8_t brightness);

#endif