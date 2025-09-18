#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef BME680_H
#define BME680_H

// Registers for BME680 Sensor
#define BME680_ADDR         0x76
#define BME680_ID           0xD0
#define BME680_CONFIG       0x75
#define BME680_CTRL_MEAS    0x74
#define BME680_CTRL_HUM     0x72
#define BME680_CTRL_GAS_0   0x70
#define BME680_CTRL_GAS_1   0x71
#define BME680_GAS_WAIT_0   0x64
#define BME680_RES_HEAT_0   0x5A
#define BME680_GAS_R        0x2A      
#define BME680_HUM          0x25
#define BME680_TEMP         0x22
#define BME680_PRESS        0x1F
#define BME680_STATUS       0x1D

// Registers for calibration parameters
#define BME680_PARAM_G      0xEB
#define BME680_RES_HEAT_VAL 0x00
#define BME680_RES_HEAT_RAN 0x02
#define BME680_PARAM_T1     0xE9
#define BME680_PARAM_T2     0x8A
#define BME680_PARAM_T3     0x8C
#define BME680_PARAM_P1     0x8E
#define BME680_PARAM_P2     0x90
#define BME680_PARAM_P3     0x92
#define BME680_PARAM_P4     0x94
#define BME680_PARAM_P5     0x96
#define BME680_PARAM_P6     0x99
#define BME680_PARAM_P7     0x98
#define BME680_PARAM_P8     0x9C
#define BME680_PARAM_P9     0x9E
#define BME680_PARAM_P10    0xA0
#define BME680_PARAM_H1     0xE2
#define BME680_PARAM_H2     0xE1
#define BME680_PARAM_H3     0xE4
#define BME680_PARAM_H4     0xE5
#define BME680_PARAM_H5     0xE6
#define BME680_PARAM_H6     0xE7
#define BME680_PARAM_H7     0xE8
#define BME680_RAN_SW_ER    0x04

typedef struct BME680_par_s{
    uint8_t g1, g3, t3, p3, p6, p7, p10, h3, h4, h5, h6, h7;
    uint16_t g2, t1, t2, p1, p2, p4, p5, p8, p9, h1, h2;
    int8_t res_heat_val, res_heat_range, gas_range, range_switching_error;
    double const_array1, const_array2;
    } BME680_par_t;

typedef struct BME680_meas_s{
    int32_t temp_adc, press_adc;
    int16_t hum_adc, gas_adc;
    double t_fine, temp_comp, press_comp, hum_comp, gas_res;
} BME680_meas_t;

bool measureBME680 (BME680_par_t * par, uint16_t target_temp, uint8_t amb_temp, int coefficient);
void tempBME680 (BME680_par_t * par, BME680_meas_t * meas);
void pressBME680 (BME680_par_t * par, BME680_meas_t * meas);
void humidityBME680 (BME680_par_t * par, BME680_meas_t * meas);
void gasResBME680 (BME680_par_t * par, BME680_meas_t * meas);
void forcedMode(void);

#endif //BME680_H