#include <stdio.h>
#include "DPS310.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

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

void readCoeffDPS310 (uint8_t * buf){
    //uint8_t buf[18] = {0};
    uint8_t reg = DPS310_COEFF;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 18, false);
    // params->c0 = (buf[0] << 4) + ((buf[1] >> 4) & 0x0F);
    // if(params->c0 > power(2, 11) - 1){
    //     params->c0 -= power(2, 12);
    // }
    
    // params->c1 = ((buf[1] << 8) & 0x0F00) + buf[2];
    // if (params->c1 > power(2,11) - 1){
    //     params->c1 -= power(2, 12);
    // }
   
    // params->c00 = ((buf[3] << 12) & 0x00FF000) + ((buf[4] << 4) & 0xFF0) + ((buf[5] >> 4) & 0x0F);
    // if (params->c00 > power(2, 19) - 1){
    //     params->c00 -= power(2, 20);
    // }
    
    // params->c10 = ((buf[5] << 16) & 0x000F0000) + ((buf[6] << 8) & 0xFF00) + buf[7];
    // if (params->c10 > power(2, 19) - 1){
    //     params->c10 -= power(2, 20);
    // }
    
    // params->c01 = ((buf[8] << 8) & 0xFF00) + buf[9];
    // if (params->c01 > power(2, 15) - 1){
    //     params->c01 -= power(2, 16);
    // }
    
    // params->c11 = ((buf[10] << 8) & 0xFF00) + buf[11];
    // if (params->c11 > power(2, 15) - 1){
    //     params->c11 -= power(2, 16);
    // }
    
    // if (params->c20 > power(2, 15) - 1){
    //     params->c20 -= power(2, 16);
    // }

    // params->c21 = ((buf[14] << 8) & 0xFF00) + buf[15];
    // if (params->c21 > power(2, 15) - 1){
    //     params->c21 -= power(2, 16);
    // }

    // params->c30 = ((buf[16] << 8) & 0xFF00) + buf[17];
    // if (params->c30 > power(2, 15) - 1){
    //     params->c30 -= power(2, 16);
    // }

}

void configPress(void){
    // Configured as 4 measurement per second and 2 times oversampling (0b_0100001) 
    uint8_t reg[] = {DPS310_PRESS_CFG, 0x21};
    uint8_t buf[1] = {0};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
}

void configTemp(void){
    // Configured as 4 measurement per second and 2 times oversampling (0b00100001)
    uint8_t reg[] = {DPS310_TEMP_CFG, 0x21};
    uint8_t buf[1] = {0};
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 2, false);
    sleep_ms(500);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg[0], 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
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

void readTemp(DPS310_meas_t * meas, DPS310_coeff_t * params){
    // According to our configuration, the compensation factor is identical = 3670016
    uint8_t buf[3] = {0};
    uint8_t regT_meas[] = {DPS310_MEAS_CFG, 0x02}; // Temperature measurement
    uint8_t regT = DPS310_TEMP_MEAS;
    // Start temperature measurement
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT_meas, 2, false);
    sleep_ms(500);
    // Reading raw Temperature Data from DPS310
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regT, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 3, false);
    meas->rawT = ((buf[0] << 16) & 0xFF0000) + ((buf[1] << 8) & 0xFF00) + buf[2];
    if (meas->rawT > power(2, 23) - 1){
        meas->rawT -= power(2, 24);
    }
    // Applying scalating factor
    meas->T = (double)meas->rawT / (double)kT;
    // Compensating parameters
    meas->T = (double)params->c0 * 0.5 + (double)params->c1 * meas->T;
}

void readPress(DPS310_meas_t * meas, DPS310_coeff_t * params){
    uint8_t buf[3] = {0};
    uint8_t regP = DPS310_PRESS_MEAS;
    uint8_t regP_meas[] = {DPS310_MEAS_CFG, 0x01}; 
    // Pressure measurement
    meas->rawT = (((double)meas->T - (double)params->c0*0.5)/(double)params->c1)* kT;
    // Start pressure measurement
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP_meas, 2, false);
    sleep_ms(500);
    // Reading raw Pressure Data from DPS310
    i2c_write_blocking(i2c_default, DPS310_ADDR, &regP, 1, true);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 3, false);
    meas->rawP = ((buf[0] << 16) & 0xFF0000) + ((buf[1] << 8) & 0xFF00) + buf[2];;
    if (meas->rawP > power(2, 23) - 1){
        meas->rawP -= power(2, 24);
    }
    // Applying scalating factor
    meas->P = meas->rawP / kT;
    // Compensating parameters
    meas->P = params->c00 + meas->P * (params->c10 + meas->P * (params->c20 + meas->P*params->c30)) + 
        (meas->rawT / kT) * params->c01 + (meas->rawT / kT) * meas->P * (params->c11 + meas->P * params->c21);
}

uint8_t idDPS310(void){
    uint8_t buf[1] = {0};
    uint8_t reg = DPS310_ID;
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, false);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 1, false);
    return buf[0];
}