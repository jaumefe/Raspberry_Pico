#include <stdio.h>
#include "DPS310.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

void correctTemp(void){
    uint8_t reg1[2] = {0x0E, 0xA5};
    uint8_t reg2[2] = {0x0F, 0x96};
    uint8_t reg3[2] = {0x62, 0x02};
    uint8_t reg4[2] = {0x0E, 0x00};
    uint8_t reg5[2] = {0x0F, 0x00};
    i2c_write_blocking(i2c_default, DPS310_ADDR, reg1, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, reg2, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, reg3, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, reg4, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, reg5, 2, false);
}

void readRawCoeffDPS310 (uint8_t * buf){
    uint8_t reg = DPS310_COEFF;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 18, false);
}

bool dps310ConfigPress(void){
    // Configured as 4 measurement per second and 2 times oversampling (0b_0100001) 
    int error;
    uint8_t reg[] = {DPS310_PRESS_CFG, 0x21};
    uint8_t buf[1] = {0};

    error = i2c_write_blocking(i2c_default, DPS310_ADDR, reg, 2, false);
    if (error == -1) {
        return false;
    }
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    error = i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    if (error == -1) {
        return false;
    }
    return true;
}

bool dps310ConfigTemp(void){
    // Configured as 4 measurement per second and 2 times oversampling (0b00100001)
    uint8_t reg[] = {DPS310_TEMP_CFG, 0x21};
    uint8_t buf[1] = {0};
    int error;
    error = i2c_write_blocking(i2c_default, DPS310_ADDR, reg, 2, false);
    if (error == -1) {
        return false;
    }
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    error = i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    if (error == -1) {
        return false;
    }
    return true;
}

bool dps310ConfigInt(void){
    // Configured as only FIFO active of all options available
    uint8_t reg[] = {DPS310_CFG, 0x02};
    uint8_t buf[1] = {0};
    int error;

    error = i2c_write_blocking(i2c_default, DPS310_ADDR, reg, 2, false);
    if (error == -1) {
        return false;
    }
    sleep_ms(500);
    reg[1] = 0x01;
    error = i2c_write_blocking(i2c_default, DPS310_ADDR, reg, 2, false);
    if (error == -1) {
        return false;
    }
    sleep_ms(500);
    reg[1] = 0x00;
    error = i2c_write_blocking(i2c_default, DPS310_ADDR, reg, 2, false);
    if (error == -1) {
        return false;
    }
    sleep_ms(500);  
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    error = i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    if (error == -1) {
        return false;
    }
    return true;
}

bool configDPS310(void){
    //Full configuration of device DPS310
    if(!dps310ConfigPress()){
        return false;
    }
    if(!dps310ConfigTemp()){
        return false;
    }
    if(!dps310ConfigInt()){
        return false;
    }
    return true;
}

void prepareMeasDPS310(void){
    uint8_t regT[] = {DPS310_MEAS_CFG, 0x02}; // Temperature measurement
    uint8_t regP[] = {DPS310_MEAS_CFG, 0x01}; // Pressure measurement
    i2c_write_blocking(i2c_default, DPS310_ADDR, regT, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, regP, 2, false);
    sleep_ms(500);
}

void dps310ReadTemp(uint8_t * buf){
    // According to our configuration, the compensation factor is identical = 3670016
    uint8_t regT_meas[] = {DPS310_MEAS_CFG, 0x02}; // Temperature measurement
    uint8_t regT = DPS310_TEMP_MEAS;
    // Start temperature measurement
    i2c_write_blocking(i2c_default, DPS310_ADDR, regT_meas, 2, false);
    sleep_ms(500);
    // Reading raw Temperature Data from DPS310
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 3, false);
}

void dps310ReadPress(uint8_t * buf){
    uint8_t regP = DPS310_PRESS_MEAS;
    uint8_t regP_meas[] = {DPS310_MEAS_CFG, 0x01}; 
    // Start pressure measurement
    i2c_write_blocking(i2c_default, DPS310_ADDR, regP_meas, 2, false);
    sleep_ms(500);
    // Reading raw Pressure Data from DPS310
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 3, false);
}

uint8_t idDPS310(void){
    uint8_t buf[1] = {0};
    uint8_t reg = DPS310_ID;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, false);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    return buf[0];
}