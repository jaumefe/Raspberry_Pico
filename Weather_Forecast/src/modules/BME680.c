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

void oversampling (void){
    //printf("Oversampling function \n");
    // Humidity oversampling set to 1x (0b00000001)
    uint8_t regH[] = {BME680_CTRL_HUM, 0x01};
    // Press and temperature oversampling set to 16x and 2x (0b01010100)
    uint8_t regM[] = {BME680_CTRL_MEAS, 0x54};
    uint8_t buf[1] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &regH, 2, false);
    sleep_ms(100);
    i2c_write_blocking(i2c_default, BME680_ADDR, &regM, 2, false);
    sleep_ms(100);
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
    uint8_t bufT1[2] = {0};
    uint8_t bufT2[2] = {0};
    uint8_t bufT3[1] = {0};
    uint8_t bufP1[2] = {0};
    uint8_t bufP2[2] = {0};
    uint8_t bufP3[1] = {0};
    uint8_t bufP4[2] = {0};
    uint8_t bufP5[2] = {0};
    uint8_t bufP6[1] = {0};
    uint8_t bufP7[1] = {0};
    uint8_t bufP8[2] = {0};
    uint8_t bufP9[2] = {0};
    uint8_t bufP10[1] = {0};
    uint8_t bufh1[2] = {0};
    uint8_t bufh2[2] = {0};
    uint8_t bufh3[1] = {0};
    uint8_t bufh4[1] = {0};
    uint8_t bufh5[1] = {0};
    uint8_t bufh6[1] = {0};
    uint8_t bufh7[1] = {0};
    uint8_t bufsw[1] = {0};

    uint8_t regP = BME680_PARAM_G;
    uint8_t regR = BME680_RES_HEAT_RAN;
    uint8_t regV = BME680_RES_HEAT_VAL;
    uint8_t regT1 = BME680_PARAM_T1;
    uint8_t regT2 = BME680_PARAM_T2;
    uint8_t regT3 = BME680_PARAM_T3;
    uint8_t regP1 = BME680_PARAM_P1;
    uint8_t regP2 = BME680_PARAM_P2;
    uint8_t regP3 = BME680_PARAM_P3;
    uint8_t regP4 = BME680_PARAM_P4;
    uint8_t regP5 = BME680_PARAM_P5;
    uint8_t regP6 = BME680_PARAM_P6;
    uint8_t regP7 = BME680_PARAM_P7;
    uint8_t regP8 = BME680_PARAM_P8;
    uint8_t regP9 = BME680_PARAM_P9;
    uint8_t regP10 = BME680_PARAM_P10;
    uint8_t regh1 = BME680_PARAM_H1;
    uint8_t regh2 = BME680_PARAM_H2;
    uint8_t regh3 = BME680_PARAM_H3;
    uint8_t regh4 = BME680_PARAM_H4;
    uint8_t regh5 = BME680_PARAM_H5;
    uint8_t regh6 = BME680_PARAM_H6;
    uint8_t regh7 = BME680_PARAM_H7;
    uint8_t regsw = BME680_RAN_SW_ER;

    // Parameters from gas measurement
    i2c_write_blocking(i2c_default, BME680_ADDR, &regP, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf, 4, false);
    par->g2 = ((buf[1] << 8) & 0xFF00) +  (buf[0] & 0x00FF);
    par->g1 = buf[2];
    par->g3 = buf[3];
    i2c_write_blocking(i2c_default, BME680_ADDR, &regR, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufR, 1, false);
    // We want only the bits 5:4 of this parameter
    par->res_heat_range = (bufR[0] >> 4) & 0x3;
    i2c_write_blocking(i2c_default, BME680_ADDR, &regV, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufV, 1, false);
    par->res_heat_val = bufV[0];

    // Parameters of temperature measurement
    i2c_write_blocking(i2c_default, BME680_ADDR, &regT1, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufT1, 2, false);
    par->t1 = ((bufT1[1] << 8) & 0xFF00) + (bufT1[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regT2, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufT2, 2, false);
    par->t2 = ((bufT2[1] << 8) & 0xFF00) + (bufT2[0] & 0x00FF);
    
    i2c_write_blocking(i2c_default, BME680_ADDR, &regT3, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufT3, 1, false);
    par->t3 = bufT3[0];

    // Parameters of pressure measurement
    i2c_write_blocking(i2c_default, BME680_ADDR, &regP1, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP1, 2, false);
    par->p1 = ((bufP1[1] << 8) & 0xFF00) + (bufP1[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP2, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP2, 2, false);
    par->p2 = ((bufP2[1] << 8) & 0xFF00) + (bufP2[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP3, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP3, 1, false);
    par->p3 = bufP3[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP4, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP4, 2, false);
    par->p4 = ((bufP4[1] << 8) & 0xFF00) + (bufP4[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP5, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP5, 2, false);
    par->p5 = ((bufP5[1] << 8) & 0xFF00) + (bufP5[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP6, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP6, 1, false);
    par->p6 = bufP6[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP7, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP7, 1, false);
    par->p7 = bufP7[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP8, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP8, 2, false);
    par->p8 = ((bufP8[1] << 8) & 0xFF00) + (bufP8[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP9, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP9, 2, false);
    par->p9 = ((bufP9[1] << 8) & 0xFF00) + (bufP9[0] & 0x00FF);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regP10, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufP10, 1, false);
    par->p10 = bufP10[0];

    // Parameters of humidity measurement
    i2c_write_blocking(i2c_default, BME680_ADDR, &regh1, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh1, 2, false);
    par->h1 = ((bufh1[0] >> 4) & 0x0F) + ((bufh1[1] << 4) & 0xFF0);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regh2, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh2, 2, false);
    par->h2 = ((bufh2[1] >> 4) & 0x0F) + ((bufh2[0] << 4) & 0xFF0);

    i2c_write_blocking(i2c_default, BME680_ADDR, &regh3, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh3, 1, false);
    par->h3 = bufh3[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regh4, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh4, 1, false);
    par->h4 = bufh4[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regh5, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh5, 1, false);
    par->h5 = bufh5[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regh6, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh6, 1, false);
    par->h6 = bufh6[0];

    i2c_write_blocking(i2c_default, BME680_ADDR, &regh7, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufh7, 1, false);
    par->h7 = bufh7[0];

    // Parameters of gas measurement
    i2c_write_blocking(i2c_default, BME680_ADDR, &regsw, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, bufsw, 1, false);
    par->range_switching_error = bufsw[0];
}

void heaterSetPoint (struct BME680_par * par, uint16_t target_temp, uint8_t amb_temp){
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
    uint8_t reg[] = {BME680_CTRL_MEAS, 0x55};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 2, false);
}

bool measureBME680 (struct BME680_par * par, uint16_t target_temp, uint8_t amb_temp, int coefficient){
    bool filt;
    oversampling();
    filt = filter(coefficient);
    getParam(par);
    heaterSetPoint(par, target_temp, amb_temp);
    heaterSettings(par);
    return filt;
}

void tempBME680 (struct BME680_par * par, struct BME680_meas * meas){
    uint8_t reg = BME680_TEMP;
    uint8_t buf[3] = {0};
    double var1, var2;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf, 3, false);
    meas->temp_adc = ((buf[2] >> 4) & 0x0F) + ((buf[1] << 4) & 0xFF0) + ((buf[0] << 12) & 0x0FF000);
    var1 = (((double)meas->temp_adc / 16384.0) - ((double)par->t1 / 1024.0)) * (double)par->t2;
    var2 = ((((double)meas->temp_adc / 131072.0) - ((double)par->t1 / 8192.0)) * (((double)meas->temp_adc / 131072.0) - ((double)par->t1 / 8192.0))) * ((double)par->t3 * 16.0);
    meas->t_fine = var1 + var2;
    meas->temp_comp = meas->t_fine / 5120.0;
}

void pressBME680 (struct BME680_par * par, struct BME680_meas * meas){
    uint8_t reg = BME680_PRESS;
    uint8_t buf[3] = {0};
    double var1, var2, var3;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf, 3, false);
    meas->press_adc = ((buf[2] >> 4) & 0x0F) + ((buf[1] << 4) & 0xFF0) + ((buf[0] << 12) & 0x0FF000);
    var1 = ((double)meas->t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)par->p6 / 131072.0);
    var2 = var2 + (var1 * (double)par->p5 * 2.0);
    var2 = (var2 / 4.0) + ((double)par->p4 * 65536.0);
    var1 = ((((double)par->p3 * var1 * var1) / 16384.0) + ((double)par->p2 * var1)) / 524288.0;
    var1 = (1.0 + (var1 / 32768.0)) * (double)par->p1;
    meas->press_comp = 1048576.0 - (double)meas->press_adc;
    meas->press_comp = ((meas->press_comp - (var2 / 4096.0)) * 6250.0) / var1;
    var1 = ((double)par->p9 * meas->press_comp * meas->press_comp) / 2147483648.0;
    var2 = meas->press_comp * ((double)par->p8 / 32768.0);
    var3 = (meas->press_comp / 256.0) * (meas->press_comp / 256.0) * (meas->press_comp / 256.0) * (par->p10 / 131072.0);
    meas->press_comp = meas->press_comp + (var1 + var2 + var3 + ((double)par->p7 * 128.0)) / 16.0;
}

void humidityBME680 (struct BME680_par * par, struct BME680_meas * meas){
    uint8_t reg = BME680_HUM;
    uint8_t buf[2] = {0};
    double var1, var2, var3, var4;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf, 2, false);
    meas->hum_adc = (buf[1] & 0x00FF) + ((buf[0] << 8) & 0xFF00);
    var1 = meas->hum_adc - (((double)par->h1 * 16.0) + (((double)par->h3 / 2.0) * meas->temp_comp));
    var2 = var1 * (((double)par->h2 / 262144.0) * (1.0 + (((double)par->h4 / 16384.0) * meas->temp_comp) + (((double)par->h5 / 1048576.0) * meas->temp_comp * meas->temp_comp)));
    var3 = (double)par->h6 / 16384.0;
    var4 = (double)par->h7 / 2097152.0;
    meas->hum_comp = var2 + ((var3 + (var4 * meas->temp_comp)) * var2 * var2);
}

void gasResBME680 (struct BME680_par * par, struct BME680_meas * meas){
    uint8_t reg = BME680_GAS_R;
    uint8_t buf[2] = {0};
    double var1;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf, 2, false);
    meas->gas_adc = ((buf[0] << 2) & 0x3FC) + ((buf[1] >> 6) & 0x3);
    par->gas_range = buf[1] & 0x0F;
    constDetBME680(par);
    var1 = (1340.0 + 5.0 * par->range_switching_error) * par->const_array1;
    meas->gas_res = var1 * par->const_array2 / (meas->gas_adc - 512.0 + var1);
}

void constDetBME680 (struct BME680_par * par){
    double const_array1[] = {1, 1, 1, 1, 1, 0.99, 1, 0.992, 1, 1, 0.998, 0.995, 1, 0.99, 1, 1};
    double const_array2[] = {8000000, 4000000, 2000000, 1000000, 499500.4995, 248262.1648, 125000, 63004.03226, 31281.28128, 15625, 7812.5, 3906.25, 1953.125, 976.5625, 488.28125, 244.140625};
    par->const_array1 = const_array1[par->gas_range];
    par->const_array2 = const_array2[par->gas_range];
}