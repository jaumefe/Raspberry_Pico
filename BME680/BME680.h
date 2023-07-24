#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef BME680_H
#define BME680_H

struct BME680_par{
    uint8_t g1, g3;
    uint16_t g2;
    int8_t res_heat_val, res_heat_range;
    };

struct BME680_meas{
    int32_t rawT, rawP;
    double T, P;
    };

bool measureBME680 (struct BME680_par * par, uint16_t target_temp, uint8_t amb_temp, int coefficient);

#endif //BME680_H