#include <stdio.h>
#include "basic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef DPS310_H
#define DPS310_H

struct DPS310_coeff{
    int16_t c0, c1, c01, c11, c20, c21, c30;
    int32_t c00, c10;
    };

struct DPS310_meas{
    int32_t rawT, rawP;
    double T, P;
    };

void readCoeffDPS310 (struct DPS310_coeff * params);

void configPress(void);

void configTemp(void);

void configInt(void);

void configDPS310(void);

void startupDPS310(void);

void readTemp(struct DPS310_meas * meas, struct DPS310_coeff * params);

void readPress(struct DPS310_meas * meas, struct DPS310_coeff * params);

uint8_t idDPS310(void);

void correctTemp(void);
#endif //DPS310_H