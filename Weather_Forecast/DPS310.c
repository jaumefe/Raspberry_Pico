#include <stdio.h>
#include "basic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

//Registers for pressure sensor (DPS310)
#define DPS310_ADDR         0x77
#define DPS310_COEFF        0x10
#define DPS310_PRESS_CFG    0x06
#define DPS310_TEMP_CFG     0x07
#define DPS310_MEAS_CFG     0x08
#define DPS310_CFG          0x09
#define DPS310_ID           0x0D
#define DPS310_PRESS_MEAS   0x00

struct DPS310_coeff {
    uint16_t c0, c1, c01,c11, c20, c21, c30;
    uint32_t c00, c10;
};

struct DPS310_meas {
    uint32_t rawT, rawP;
};

void readCoeffDPS310 (struct DPS310_coeff * params){
    uint8_t buf[10] = {0};
    uint8_t reg = DPS310_COEFF;
    i2c_write_blocking(i2c, DPS310_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, DPS310_ADDR, buf, 10, false);
    //pseudocode c0 = (buf[0] * 2⁴) + ((buf[1] / 2⁴) AND 0x0F)
    params->c0 = (buf[0] << 4) + ((buf[1] >> 4) & 0x0F);
    if(params->c0 > power(2, 11) - 1){
        params->c0 -= power(2, 12);
    }
    params->c1 = ((buf[1] << 8) & 0x0F00) + buf[2];
    if (params->c1 > power(2,11) - 1){
        params->c1 -= power(2, 12);
    }
    params->c00 = ((buf[3] << 12) & 0x00FF0000) + ((buf[4] << 8) & 0xFF00) + ((buf[5] >> 4) & 0x0F);
    if (params->c00 > power(2, 19) - 1){
        params->c00 -= power(2, 20);
    }
    params->c10 = ((buf[5] << 16) & 0x000F0000) + ((buf[6] << 8) & 0xFF00) + buf[7];
    if (params->c10 > power(2, 19) - 1){
        params->c10 -= power(2, 20);
    }
    params->c01 = ((buf[8] << 8) & 0xFF00) + buf[9];
    if (params->c01 > power(2, 7) - 1){
        params->c01 -= power(2, 8);
    }
    params->c11 = ((buf[10] << 8) & 0xFF00) + buf[11];
    if (params->c11 > power(2, 7) - 1){
        params->c11 -= power(2, 8);
    }
    params->c10 = ((buf[12] << 8) & 0xFF00) + buf[13];
    if (params->c10 > power(2, 7) - 1){
        params->c10 -= power(2, 8);
    }
    params->c20 = ((buf[14] << 8) & 0xFF00) + buf[15];
    if (params->c20 > power(2, 7) - 1){
        params->c20 -= power(2, 8);
    }
    params->c21 = ((buf[16] << 8) & 0xFF00) + buf[17];
    if (params->c21 > power(2, 7) - 1){
        params->c21 -= power(2, 8);
    }
    params->c30 = ((buf[18] << 8) & 0xFF00) + buf[19];
    if (params->c30 > power(2, 7) - 1){
        params->c30 -= power(2, 8);
    }
}

void configPress(){
    // Configured as 4 measurement per second and 2 times oversampling (0b_0100001) 
    uint8_t reg[] = {DPS310_PRESS_CFG, 0x21};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
}

void configTemp(){
    // Configured as 4 measurement per second and 2 times oversampling (0b00100001)
    uint8_t reg[] = {DPS310_TEMP_CFG, 0x21};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
}

void configInt(){
    // COnfigured as only FIFO active of all options available
    uint8_t reg[] = {DPS310_CFG, 0x02};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
}

void configDPS310(){
    //Full configuration of device DPS310
    configPress();
    configTemp();
    configInt();
}

void startupDPS310(){
    uint8_t regT[] = {DPS310_MEAS_CFG, 0x02}; // Temperature measurement
    uint8_t regP[] = {DPS310_MEAS_CFG, 0x01}; // Pressure measurement
    configDPS310();
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT, 2, false);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP, 2, false);
}

void readTemp (struct DPS310_meas * params){
    uint8_t regT[] = {};
}