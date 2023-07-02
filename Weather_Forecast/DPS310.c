#include <stdio.h>
#include "basic.h"
#include "DPS310.h"
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
#define DPS310_TEMP_MEAS    0x03

// Coefficients for correcting internal error of the sensor
#define COEFF_1             0x45
#define COEFF_2             0x96
#define COEFF_3             0x92
#define COEFF_4             0x00
#define COEFF_5             0x00

#define kT                  3670016

void correctTemp(void){
    uint8_t reg1 = {0x0E, 0xA5};
    uint8_t reg2 = {0x0F, 0x96};
    uint8_t reg3 = {0x62, 0x02};
    uint8_t reg4 = {0x0E, 0x00};
    uint8_t reg5 = {0x0F, 0x00};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg1, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg2, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg3, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg4, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg5, 2, false);
}

void readCoeffDPS310 (struct DPS310_coeff * params){
    uint8_t buf[18] = {0};
    uint8_t reg = DPS310_COEFF;
    printf("punt3.1\n");
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 20, false);
    printf("buf[0] %d\n", buf[0]);
    printf("buf[1] %d\n", buf[1]);
    printf("buf[2] %d\n", buf[2]);
    printf("buf[3] %d\n", buf[3]);
    printf("buf[4] %d\n", buf[4]);
    printf("buf[5] %d\n", buf[5]);
    printf("buf[6] %d\n", buf[6]);
    printf("buf[7] %d\n", buf[7]);
    printf("buf[8] %d\n", buf[8]);
    printf("buf[9] %d\n", buf[9]);
    printf("buf[10] %d\n", buf[10]);
    printf("buf[11] %d\n", buf[11]);
    printf("buf[12] %d\n", buf[12]);
    printf("buf[13] %d\n", buf[13]);
    printf("buf[14] %d\n", buf[14]);
    printf("buf[15] %d\n", buf[15]);
    printf("buf[16] %d\n", buf[16]);
    printf("buf[17] %d\n", buf[17]);
    //pseudocode c0 = (buf[0] * 2⁴) + ((buf[1] / 2⁴) AND 0x0F)
    params->c0 = (buf[0] << 4) + ((buf[1] >> 4) & 0x0F);
    if(params->c0 > power(2, 11) - 1){
        params->c0 -= power(2, 12);
    }
    printf("c0 %d\n", params->c0);
    params->c1 = ((buf[1] << 8) & 0x0F00) + buf[2];
    if (params->c1 > power(2,11) - 1){
        params->c1 -= power(2, 12);
    }
    printf("c1 %d\n", params->c1);
    params->c00 = ((buf[3] << 12) & 0x00FF000) + ((buf[4] << 4) & 0xFF0) + ((buf[5] >> 4) & 0x0F);
    if (params->c00 > power(2, 19) - 1){
        params->c00 -= power(2, 20);
    }
    printf("c00 %d\n", params->c00);
    params->c10 = ((buf[5] << 16) & 0x000F0000) + ((buf[6] << 8) & 0xFF00) + buf[7];
    if (params->c10 > power(2, 19) - 1){
        params->c10 -= power(2, 20);
    }
    printf("c10 %d\n", params->c10);
    params->c01 = ((buf[8] << 8) & 0xFF00) + buf[9];
    if (params->c01 > power(2, 15) - 1){
        params->c01 -= power(2, 16);
    }
    printf("c01 %d\n", params->c01);
    params->c11 = ((buf[10] << 8) & 0xFF00) + buf[11];
    if (params->c11 > power(2, 15) - 1){
        params->c11 -= power(2, 16);
    }
    printf("c11 %d\n", params->c11);
    params->c20 = ((buf[12] << 8) & 0xFF00) + buf[13];
    if (params->c20 > power(2, 15) - 1){
        params->c20 -= power(2, 16);
    }
    printf("c20 %d\n", params->c20);
    params->c21 = ((buf[14] << 8) & 0xFF00) + buf[15];
    if (params->c21 > power(2, 15) - 1){
        params->c21 -= power(2, 16);
    }
    printf("c21 %d\n", params->c21);
    params->c30 = ((buf[16] << 8) & 0xFF00) + buf[17];
    if (params->c30 > power(2, 15) - 1){
        params->c30 -= power(2, 16);
    }
    printf("c30 %d\n", params->c30);
}

void configPress(void){
    // Configured as 4 measurement per second and 2 times oversampling (0b_0100001) 
    uint8_t reg[] = {DPS310_PRESS_CFG, 0x21};
    uint8_t buf[1] = {0};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    printf("Reg: 0x06 -> %d\n", buf[0]);
}

void configTemp(void){
    // Configured as 4 measurement per second and 2 times oversampling (0b00100001)
    uint8_t reg[] = {DPS310_TEMP_CFG, 0x21};
    uint8_t buf[1] = {0};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    printf("buf[0] %d\n", buf[0]);
}

void configInt(void){
    // COnfigured as only FIFO active of all options available
    uint8_t reg[] = {DPS310_CFG, 0x02};
    uint8_t buf[1] = {0};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);
    reg[1] = 0x01;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);
    reg[1] = 0x00;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);  
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    printf("buf[0] %d\n", buf[0]);
}

void configDPS310(void){
    //Full configuration of device DPS310
    configPress();
    configTemp();
    configInt();
}

void startupDPS310(void){
    uint8_t regT[] = {DPS310_MEAS_CFG, 0x02}; // Temperature measurement
    uint8_t regP[] = {DPS310_MEAS_CFG, 0x01}; // Pressure measurement
    configDPS310();
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP, 2, false);
    sleep_ms(500);
}

void readTemp(struct DPS310_meas * meas, struct DPS310_coeff * params){
    // According to our configuration, the compensation factor is identical = 3670016
    printf("punt 4.1.1\n");
    uint8_t buf[3] = {0};
    uint8_t regT_meas[] = {DPS310_MEAS_CFG, 0x02}; // Temperature measurement
    uint8_t regT = DPS310_TEMP_MEAS;
    // Start temperature measurement
    printf("Measuring temperature\n");
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT_meas, 2, false);
    sleep_ms(500);
    // Reading raw Temperature Data from DPS310
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 3, false);
    printf("punt 4.1.2\n");
    printf("buf[0] %d\n", buf[0]);
    printf("buf[1] %d\n", buf[1]);
    printf("buf[2] %d\n", buf[2]);
    meas->rawT = ((buf[0] << 16) & 0xFF0000) + ((buf[1] << 8) & 0xFF00) + buf[2];
    printf("rawT %d\n", meas->rawT);
    if (meas->rawT > power(2, 23) - 1){
        meas->rawT -= power(2, 24);
    }
    // Applying scalating factor
    meas->T = (double)meas->rawT / (double)kT;
    printf("T %f\n", meas->T);
    // Compensating parameters
    meas->T = (double)params->c0 * 0.5 + (double)params->c1 * meas->T;
    printf("T %f\n", meas->T);
    printf("punt 4.1.3\n");
}

void readPress(struct DPS310_meas * meas, struct DPS310_coeff * params){
    printf("punt 4.1\n");
    uint8_t buf[3] = {0};
    uint8_t regP = DPS310_PRESS_MEAS;
    uint8_t regP_meas[] = {DPS310_MEAS_CFG, 0x01}; // Pressure measurement
    printf("bufP[0] %d\n", buf[0]);
    printf("bufP[1] %d\n", buf[1]);
    printf("bufP[2] %d\n", buf[2]);
    // First, reading temperature for compensating Pressure values
    correctTemp();
    readTemp(meas, params);
    printf("T %f\n", meas->T);
    printf("Punt 4.2\n");
    printf("bufP[0] %d\n", buf[0]);
    printf("bufP[1] %d\n", buf[1]);
    printf("bufP[2] %d\n", buf[2]);
    // Start pressure measurement
    printf("Measuring pressure\n");
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP_meas, 2, false);
    sleep_ms(500);
    // Reading raw Pressure Data from DPS310
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 3, false);
    printf("Punt 4.3\n");
    printf("buf[0] %d\n", buf[0]);
    printf("buf[1] %d\n", buf[1]);
    printf("buf[2] %d\n", buf[2]);
    printf("kT %d\n", kT);
    meas->rawP = ((buf[0] << 16) & 0xFF0000) + ((buf[1] << 8) & 0xFF00) + buf[2];
    printf("Punt 4.4\n");
    if (meas->rawP > power(2, 23) - 1){
        meas->rawP -= power(2, 24);
    }
    // Applying scalating factor
    meas->P = meas->rawP / kT;
    printf("Punt 4.5\n");
    // Compensating parameters
    meas->P = params->c00 + meas->P * (params->c10 + meas->P * (params->c20 + meas->P*params->c30)) + 
        (meas->rawT / kT) * params->c01 + (meas->rawT / kT) * meas->P * (params->c11 + meas->P * params->c21);
    printf("Punt 4.6\n");
    printf("P %f\n", meas->P);
}

uint8_t idDPS310(void){
    uint8_t buf[1] = {0};
    uint8_t reg = DPS310_ID;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, false);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    return buf[0];
}