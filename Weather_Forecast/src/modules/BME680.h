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
#define BME680_CTRL_MEAS    _u(0x74)
#define BME680_CTRL_HUM     _u(0x72)
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

// Settings
#define BME680_OSRS_H       0b001   // Humidity oversampling x1
#define BME680_OSRS_T       0b010   // Temperature oversampling x2
#define BME680_OSRS_P       0b101   // Pressure oversampling x16
#define BME680_MODE_FORCED  0b01    // Forced mode
#define BME680_MODE_SLEEP   0b00    // Sleep mode
#define BME680_FILTER_0     0b000   // Filter off
#define BME680_FILTER_1     0b001   // Filter coefficient 1
#define BME680_FILTER_3     0b010   // Filter coefficient 3
#define BME680_FILTER_7     0b011   // Filter coefficient 7
#define BME680_FILTER_15    0b100   // Filter coefficient 15
#define BME680_FILTER_31    0b101   // Filter coefficient 31
#define BME680_FILTER_63    0b110   // Filter coefficient 63
#define BME680_FILTER_127   0b111   // Filter coefficient 127
#define BME680_HEATER_100MS 0x59    // Heater duration 100ms
#define BME680_RUN_GAS      0x1     // Enable gas sensor
#define BME680_NB_CONV      0b0000  // Select heater configuration 0

#define TARGET_HEATER_TEMP  300     // Heater temperature 300°C
#define AMBIENT_TEMP        25      // Ambient temperature 25°C 


typedef struct BME680_meas_s{
    int32_t temp_adc, press_adc;
    int16_t hum_adc, gas_adc;
    double t_fine, temp_comp, press_comp, hum_comp, gas_res;
} BME680_meas_t;

typedef struct bme680_temp_par_s {
    uint16_t t1;
    int16_t t2;
    int8_t t3;
} bme680_temp_par_t;

typedef struct bme680_press_par_s {
    uint16_t p1;
    int16_t p2, p4, p5, p8, p9;
    int8_t p3, p6, p7, p10;
} bme680_press_par_t;

typedef struct bme680_hum_par_s {
    uint16_t h1, h2;
    int8_t h3, h4, h5, h7;
    uint8_t h6;
} bme680_hum_par_t;

// void measureBME680 (BME680_par_t * par, uint16_t target_temp, uint8_t amb_temp, int coefficient);
void bme680Temperature (uint8_t * temp_buf);
void bme680Pressure (uint8_t * press_buf);
void bme680Humidity (uint8_t * hum_buf);
void bme680GasRes (uint8_t * gas_buf);
void forcedMode(void);
void oversampling (void);
void bme680GetCalibrationParameters (bme680_temp_par_t * temp_par, bme680_press_par_t * press_par, bme680_hum_par_t * hum_par);

#endif //BME680_H