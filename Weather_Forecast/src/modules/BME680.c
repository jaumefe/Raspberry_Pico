#include <stdio.h>
#include "BME680.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

void oversampling (void){
    // First read the current value of ctrl_hum register
    uint8_t reg = BME680_CTRL_HUM;
    uint8_t bufH;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, &bufH, 1, false);
    
    // Set the oversampling for humidity to x1 (0b001)
    uint8_t regH[] = {BME680_CTRL_HUM, bufH | BME680_OSRS_H};
    i2c_write_blocking(i2c_default, BME680_ADDR, regH, 2, false);
    
    // Temperature and pressure oversampling set to 2x and 16x (0b01010100) (Sleep mode)
    uint8_t regPT[] = {BME680_CTRL_MEAS, (BME680_OSRS_T << 5) | (BME680_OSRS_P << 2) | BME680_MODE_SLEEP};
    i2c_write_blocking(i2c_default, BME680_ADDR, regPT, 2, false);
}

void filter (int coefficient){
    uint8_t reg[2] = {};
    reg[0] = BME680_CONFIG;

    uint8_t buf;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg[0], 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, &buf, 1, false);

    uint8_t filter = 0;
    switch (coefficient)
    {
        case 1:
            filter = BME680_FILTER_1;
        case 3:
            filter = BME680_FILTER_3;
        case 7:
            filter = BME680_FILTER_7;
        case 15:
            filter = BME680_FILTER_15;
        case 31:
            filter = BME680_FILTER_31;
        case 63:
            filter = BME680_FILTER_63;
        case 127:
            filter = BME680_FILTER_127;
        default:
            filter = BME680_FILTER_0;
    }
    reg[1] = buf | (filter << 2);
    i2c_write_blocking(i2c_default, BME680_ADDR, reg, 2, false);
}

void bme680GetCalibrationParameters (bme680_temp_par_t * temp_par, bme680_press_par_t * press_par, bme680_hum_par_t * hum_par){
    // Temperature calibration parameters
    uint8_t reg = BME680_PARAM_T1;
    uint8_t buf_temp_par_t1[2] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf_temp_par_t1, 2, false);
    temp_par->t1 = (uint16_t)(((buf_temp_par_t1[1] << 8) & 0xFF00) | (buf_temp_par_t1[0] & 0x00FF));

    reg = BME680_PARAM_T2;
    uint8_t buf_temp_par_t2_t3[3] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf_temp_par_t2_t3, 3, false);
    temp_par->t2 = (int16_t)(((buf_temp_par_t2_t3[1] << 8) & 0xFF00) | (buf_temp_par_t2_t3[0] & 0x00FF));
    temp_par->t3 = (int8_t)buf_temp_par_t2_t3[2];

    // Pressure calibration parameters
    reg = BME680_PARAM_P1;
    uint8_t buf_press_par_p1_p3[5] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf_press_par_p1_p3, 5, false);
    press_par->p1 = (uint16_t)((buf_press_par_p1_p3[0] & 0x00FF) | ((buf_press_par_p1_p3[1] << 8)& 0xFF00));
    press_par->p2 = (int16_t)((buf_press_par_p1_p3[2] & 0x00FF) | ((buf_press_par_p1_p3[3] << 8)& 0xFF00));
    press_par->p3 = (int8_t)buf_press_par_p1_p3[4];

    reg = BME680_PARAM_P4;
    uint8_t buf_press_par_p4_p7[6] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf_press_par_p4_p7, 6, false);
    press_par->p4 = (int16_t)((buf_press_par_p4_p7[0] & 0x00FF) | ((buf_press_par_p4_p7[1] << 8)& 0xFF00));
    press_par->p5 = (int16_t)((buf_press_par_p4_p7[2] & 0x00FF) | ((buf_press_par_p4_p7[3] << 8)& 0xFF00));
    press_par->p6 = (int8_t)buf_press_par_p4_p7[5];
    press_par->p7 = (int8_t)buf_press_par_p4_p7[4];

    reg = BME680_PARAM_P8;
    uint8_t buf_press_par_p8_p10[5] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf_press_par_p8_p10, 5, false);
    press_par->p8 = (int16_t)((buf_press_par_p8_p10[0] & 0x00FF) | ((buf_press_par_p8_p10[1] << 8)& 0xFF00));
    press_par->p9 = (int16_t)((buf_press_par_p8_p10[2] & 0x00FF) | ((buf_press_par_p8_p10[3] << 8)& 0xFF00));
    press_par->p10 = (int8_t)buf_press_par_p8_p10[4];

    // Humidity parameters
    reg = BME680_PARAM_H2;
    uint8_t buf_hum_par_h[8] = {0};
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, buf_hum_par_h, 8, false);
    hum_par->h1 = (uint16_t)(((buf_hum_par_h[2] << 4) & 0xFF00) | (buf_hum_par_h[1] & 0x0F));
    hum_par->h2 = (uint16_t)(((buf_hum_par_h[0] << 4) & 0xFF00) | ((buf_hum_par_h[1] >> 2) & 0x0F));
    hum_par->h3 = (int8_t)buf_hum_par_h[3];
    hum_par->h4 = (int8_t)buf_hum_par_h[4];
    hum_par->h5 = (int8_t)buf_hum_par_h[5];
    hum_par->h6 = (uint8_t)buf_hum_par_h[6];
    hum_par->h7 = (int8_t)buf_hum_par_h[7];
}

uint8_t computeHeaterResistance (uint16_t target_temp, uint8_t amb_temp){
    // Get calibration parameters
    uint8_t par_g[4] = {0};
    uint8_t reg_par_g = BME680_PARAM_G;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg_par_g, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, par_g, 4, false);
    int8_t par_g1, par_g3;
    int16_t par_g2;
    par_g2 = ((par_g[1] << 8) & 0xFF00) + (par_g[0] & 0x00FF);
    par_g1 = par_g[2];
    par_g3 = par_g[3];

    // Get the heater range stored in address 0x02 [5:4]
    uint8_t res_heat_range;
    uint8_t reg_rhr = BME680_RES_HEAT_RAN;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg_rhr, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, &res_heat_range, 1, false);
    res_heat_range = (res_heat_range >> 4) & 0x03;

    // Get the heater correction factor (0x00)
    uint8_t buf_heat_val;
    uint8_t reg_rhv = BME680_RES_HEAT_VAL;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg_rhv, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, &buf_heat_val, 1, false);
    int8_t res_heat_val = (int8_t)buf_heat_val;

    // Calculate the heater resistance value
    uint8_t res_heat_x;
    double var1, var2, var3, var4, var5;
    var1 = ((double)par_g1 / 16.0) + 49.0;
    var2 = (((double)par_g2 / 32768.0) * 0.0005) + 0.00235;
    var3 = (double)par_g3 / 1024.0;
    var4 = var1 * (1.0 + (var2 * (double) target_temp));
    var5 = var4 + (var3 * (double)amb_temp);
    res_heat_x = (uint8_t)(3.4 * ((var5 * (4.0 / (4.0 + (double)res_heat_range)) * (1.0/(1.0 + ((double)res_heat_val * 0.002)))) - 25));

    return res_heat_x;
}

void configureGasMeasurement (void) {
    // Set heater duration to 100ms
    uint8_t reg_gas_wait_0[] = {BME680_GAS_WAIT_0, BME680_HEATER_100MS};
    i2c_write_blocking(i2c_default, BME680_ADDR, reg_gas_wait_0, 2, false);

    // Set heater temperature to 300ºC
    // Page 22: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme680-ds001.pdf
    uint8_t res_heat_0 = computeHeaterResistance(TARGET_HEATER_TEMP, AMBIENT_TEMP);
    uint8_t reg_res_heat_0[] = {BME680_RES_HEAT_0, res_heat_0};
    i2c_write_blocking(i2c_default, BME680_ADDR, reg_res_heat_0, 2, false);

    // Enable gas measurements and set heater configuration to profile 0
    uint8_t reg_ctrl_gas_1[] = {BME680_CTRL_GAS_1, (BME680_RUN_GAS << 4) | BME680_NB_CONV};
    i2c_write_blocking(i2c_default, BME680_ADDR, reg_ctrl_gas_1, 2, false);
}

void forcedMode (void){
    uint8_t reg[] = {BME680_CTRL_MEAS, 0x55};
    i2c_write_blocking(i2c_default, BME680_ADDR, reg, 2, false);
}

// void measureBME680 (BME680_par_t * par, uint16_t target_temp, uint8_t amb_temp, int coefficient){
//     oversampling();
//     filter(coefficient);
//     bme680GetCalibrationParameters(par);
//     heaterSetPoint(par, target_temp, amb_temp); 
//     heaterSettings(par);
// }

void bme680Temperature (uint8_t * temp_buf){
    uint8_t reg = BME680_TEMP;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, temp_buf, 3, false);
}

void bme680Pressure (uint8_t * press_buf){
    uint8_t reg = BME680_PRESS;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, press_buf, 3, false);
}

void bme680Humidity (uint8_t * hum_buf){
    uint8_t reg = BME680_HUM;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, hum_buf, 2, false);
}

void bme680GasRes (uint8_t * gas_buf){
    uint8_t reg = BME680_GAS_R;
    i2c_write_blocking(i2c_default, BME680_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, BME680_ADDR, gas_buf, 2, false);
}

// void constDetBME680 (BME680_par_t * par){
//     double const_array1[] = {1, 1, 1, 1, 1, 0.99, 1, 0.992, 1, 1, 0.998, 0.995, 1, 0.99, 1, 1};
//     double const_array2[] = {8000000, 4000000, 2000000, 1000000, 499500.4995, 248262.1648, 125000, 63004.03226, 31281.28128, 15625, 7812.5, 3906.25, 1953.125, 976.5625, 488.28125, 244.140625};
//     par->const_array1 = const_array1[par->gas_range];
//     par->const_array2 = const_array2[par->gas_range];
// }