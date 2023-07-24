#include <stdio.h>
#include "BME680.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

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

void oversampling (void){
    // Humidity oversampling set to 1x (0b00000001)
    uint8_t regH[] = {BME680_CTRL_HUM, 0x01};
    // Press and temperature oversampling set to 2x (0b01010100)
    uint8_t regM[] = {BME680_CTRL_MEAS, 0x54};
    i2c_write_blocking(i2c_default, BME680_ADDR, &regH, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, BME680_ADDR, &regM, 2, false);
    sleep_ms(500);
}

bool filter (int coefficient){
    uint8_t reg[2] = {};
    switch (coefficient)
    {
        case 1:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x04;
            break;
        case 3:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x08;
            break;
        case 7:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x0C;
            break;
        case 15:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x10;
            break;
        case 31:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x14;
            break;
        case 63:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x18;
            break;
        case 127:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x1C;
            break;
        default:
            reg[0] = BME680_CONFIG;
            reg[1] = 0x00;
            i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 2, false);
            return false;
    }
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 2, false);
    return true;
}

void getParam (struct BME680_par * par){
    uint8_t buf[4] = {0};
    uint8_t bufR[1] = {0};
    uint8_t bufV[1] = {0};
    uint8_t regP = BME680_PARAM_G;
    uint8_t regR = BME680_RES_HEAT_RAN;
    uint8_t regV = BME680_RES_HEAT_VAL;
    i2c_write_blocking(i2c_default, BME680_ADDR, &regP, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf, 4, false);
    par->g2 = ((buf[0] << 8) & 0xF0) +  ((buf[1] >> 8) & 0x0F);
    par->g1 = buf[2];
    par->g3 = buf[3];
    i2c_write_blocking(i2c_default, BME680_ADDR, &regR, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufR, 1, false);
    // We want only the bits 5:4 of this parameter
    par->res_heat_range = (bufR[0] >> 5) & 0x3;
    i2c_write_blocking(i2c_default, BME680_ADDR, &regV, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufV, 1, false);
    par->res_heat_val = bufV[0];
}

void heaterSetPoint (struct BME680_par * par, uint8_t target_temp, uint8_t amb_temp){
    uint8_t regWait[] = {BME680_GAS_WAIT_0, 0x59};
    double var1, var2, var3, var4, var5;
    uint8_t res_heat_x;
    uint8_t reg[2] = {0};
    var1 = ((double)par->g1 / 16.0) + 49.0;
    var2 = (((double)par->g2 / 32768.0) * 0.0005) + 0.00235;
    var3 = (double)par->g3 / 1024.0;
    var4 = var1 * (1.0 + (var2 * (double)target_temp));
    var5 = var4 + var3 * (double)amb_temp;
    res_heat_x = (uint8_t)(3.4 * ((var5 * (4.0 / (4.0 + (double)par->res_heat_range)) * (1.0/(1.0 + ((double)par->res_heat_val * 0.002)))) - 25));
    reg[0] = BME680_RES_HEAT_0;
    reg[1] = res_heat_x;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 2, false);
}

void heaterSettings (struct BME680_par * par){
    uint8_t reg[] = {BME680_CTRL_GAS_1, 0x10};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 2, false);
}

void forcedMode (void){
    uint8_t reg[] = {BME680_CTRL_MEAS, 0b01};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 2, false);
}

bool measureBME680 (struct BME680_par * par, uint16_t target_temp, uint8_t amb_temp, int coefficient){
    bool filt;
    oversampling();
    filt = filter(coefficient);
    getParam(par);
    heaterSetPoint(par, target_temp, amb_temp);
    heaterSettings(par);
    forcedMode();
    return filt;
}