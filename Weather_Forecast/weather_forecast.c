#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#define I2C_PORT i2c0

//Registers for pressure sensor (DPS310)
static const uint8_t DPS310_ADDR = 0x77;
static const uint8_t DPS310_COEFF = 0x10;
static const uint8_t DPS310_PRESS_CFG = 0x06;
static const uint8_t DPS310_TEMP_CFG = 0x07;
static const uint8_t DPS310_MEAS_CFG = 0x08;

//Register for temperature and humidty sensor (SHT)
static const uint8_t SHT4XA_ADDR = 0x44;
static const uint8_t SHT4XA_MEAS = 0xFD;

//I2C reserves some addresses for special purposes.
    //These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr){
        return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
    }

void configureI2C(){
#if !defined (i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/bus_scan example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    // In this case we will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 10 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
#endif
}

int32_t complementTwo(int32_t val, uint8_t bits){
    if (val & ((uint32_t)1 << (bits - 1))){
        val -= (uint32_t)1 << bits;
    }
    return val;
}

int reg_write(i2c_inst_t *i2c, const uint8_t addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes){
    int num_bytes_read = 0;
    uint8_t msg[nbytes + 1];

    //Check to make sure that we are going to send at least 1 byte
    if (nbytes < 1){
        return 0;
    }

    //Append register address to front of packet data
    msg[0] = reg;
    for (int i = 1; i < (nbytes + 1); i++){
        msg[i] = buf[i - 1];
    }

    //Write data to register over I2C
    i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

    return num_bytes_read;
}

int main (){
    //Enable UART so status output can be printed
    stdio_init_all();

    //Configuration of I2C. I2C pins are available to picotool
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    //configureI2C();

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    uint8_t coeff[18] = {0};
    uint8_t temp[6] = {0};
    uint8_t read_bytes = 0;
    int c0, c1, c00, c10, c01, c11, c20, c21, c30;
    //Variable defining the values of the registers for DPS310X configuration
    uint8_t config[2] = {0x01, 0x01};
    uint8_t dataRequest[2] = {0x02, 0x01};
    uint8_t ID[6] = {0};
    uint8_t reg = 0xFD; 
    
    /*
    // 50 ms to startup of the device
    sleep_ms(1000);

    //Asking for the correction coefficient
    i2c_write_blocking(i2c_default, DPS310_ADDR, DPS310_COEFF, 1, true);
    read_bytes = i2c_read_blocking(i2c_default, DPS310_ADDR, &coeff, 18, false);

    //Temperature coefficients (c0 and c1)
    c0 = (int)((coeff[0] << 4) | (coeff[1] >> 4) & 0x0F);
    c0 = complementTwo(c0, 12);

    c1 = (int)(((coeff[1] & 0x0F) << 4) | coeff[2]);
    c1 = complementTwo(c1, 12);

    //Pressure coefficients (c00, c10, c01, c11, c20, c21, c30)
    c00 = ((uint32_t)(coeff[3]) << 12) | ((uint32_t)(coeff[4]) << 4) | (((uint32_t)(coeff[5]) >> 4) & 0x0F);
    c00 = complementTwo(c00, 20);

    c10 = (((uint32_t)(coeff[5]) << 16) & 0x0F) | ((uint32_t)(coeff[6]) << 8) | (uint32_t)(coeff[7]);
    c10 = complementTwo(c10, 20);

    c01 = ((int)(coeff[8] << 8)) | ((int)(coeff[9]));
    c01 = complementTwo(c01, 16);

    c11 = ((int)(coeff[10] << 8)) | ((int)(coeff[11]));
    c11 = complementTwo(c11, 16);

    c20 = ((int)(coeff[12] << 8)) | ((int)(coeff[13]));
    c20 = complementTwo(c20, 16);

    c21 = ((int)(coeff[14] << 8)) | ((int)(coeff[15]));
    c21 = complementTwo(c21, 16);

    c30 = ((int)(coeff[16] << 8)) | ((int)(coeff[17]));
    c30 = complementTwo(c30, 16);

    //Configuring the DPS310 as a Weather Station (according to the datasheet)
    reg_write(i2c_default, DPS310_ADDR, DPS310_PRESS_CFG, &config[0], 1); //Pressure
    reg_write(i2c_default, DPS310_ADDR, DPS310_TEMP_CFG, &config[1], 1); //Temperature
    */

   sleep_ms(2000);
    while(1){
        printf("Initial measurement\n");
        printf("ID: %d\n", ID[0]);
        printf("ID: %d\n", ID[1]);
        printf("ID: %d\n", ID[2]);
        printf("ID: %d\n", ID[3]);
        printf("ID: %d\n", ID[4]);
        printf("ID: %d\n", ID[5]);
        sleep_ms(2000);
        i2c_write_blocking(i2c_default, SHT4XA_ADDR, &reg, 1, true);
        read_bytes = i2c_read_blocking(i2c_default, SHT4XA_ADDR, ID, 1, false);
        printf("ID: %d\n", ID[0]);
        printf("ID: %d\n", ID[1]);
        printf("ID: %d\n", ID[2]);
        printf("ID: %d\n", ID[3]);
        printf("ID: %d\n", ID[4]);
        printf("ID: %d\n", ID[5]);
        printf("Read bytes: %d\n", read_bytes);
        
        /*read_bytes = 0;
        i2c_write_blocking(i2c_default, DPS310_ADDR, 0x0D, 1, true);
        read_bytes = i2c_read_blocking(i2c_default, DPS310_ADDR, ID, 1, false);
        printf("ID: %d\n", ID);
        printf("%d\n", read_bytes);*/
        /*printf("Printing coefficients\n");
        printf("c0: %d\n", c0);
        printf("c1: %d\n", c1);
        printf("c00: %d\n", c00);
        printf("c10: %d\n", c10);
        printf("c01: %d\n", c01);
        printf("c11: %d\n", c11);
        printf("c20: %d\n", c20);
        printf("c21: %d\n", c21);
        printf("c30: %d\n", c30);*/
        sleep_ms(1000);
    }
    return 0;
}


/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Sweep through all 7-bit I2C addresses, to see if any slaves are present on
// the I2C bus. Print out a table that looks like this:
//
// I2C Bus Scan
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
// 0
// 1       @
// 2
// 3             @
// 4
// 5
// 6
// 7
//
// E.g. if slave addresses 0x12 and 0x34 were acknowledged.
/*
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
    // Enable UART so we can print status output
    stdio_init_all();

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
    return 0;
}*/