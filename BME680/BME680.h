#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef BME680_H
#define BME680_H

struct BME680_par{
    uint8_t g1, g3, t3, p3, p6, p7, p10;
    uint16_t g2, t1, t2, p1, p2, p4, p5, p8, p9;
    int8_t res_heat_val, res_heat_range;
    };

struct BME680_meas{
    int32_t temp_adc, press_adc;
    double t_fine, temp_comp, press_comp;
    };

bool measureBME680 (struct BME680_par * par, uint16_t target_temp, uint8_t amb_temp, int coefficient);

#endif //BME680_H