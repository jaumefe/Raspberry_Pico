#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef BME680_H
#define BME680_H

struct BME680_par{
    uint8_t g1, g3, t3, p3, p6, p7, p10, h3, h4, h5, h6, h7;
    uint16_t g2, t1, t2, p1, p2, p4, p5, p8, p9, h1, h2;
    int8_t res_heat_val, res_heat_range, gas_range, range_switching_error;
    double const_array1, const_array2;
    };

struct BME680_meas{
    int32_t temp_adc, press_adc;
    int16_t hum_adc, gas_adc;
    double t_fine, temp_comp, press_comp, hum_comp, gas_res;
    };

bool measureBME680 (struct BME680_par * par, uint16_t target_temp, uint8_t amb_temp, int coefficient);
void tempBME680 (struct BME680_par * par, struct BME680_meas * meas);
void pressBME680 (struct BME680_par * par, struct BME680_meas * meas);
void humidityBME680 (struct BME680_par * par, struct BME680_meas * meas);
void gasResBME680 (struct BME680_par * par, struct BME680_meas * meas);
void forcedMode(void);

#endif //BME680_H