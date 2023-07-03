// Modelo de chip utilizado SHT40-AD1B-R2: https://www.mouser.es/datasheet/2/682/Datasheet_SHT4x-3003109.pdf
#include <stdio.h>
#include "basic.h"
#include "SHT4x.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"


//Registers for pressure sensor (DPS310)
#define SHT4X_ADDR         0x44
#define SHT4X_MEAS_HIGH    0xFD
#define SHT4X_MEAS_MED     0xF6
#define SHT4X_MEAS_LOW     0xF0
#define SHT4X_ID           0x89
#define SHT4X_MAX_HEAT_1   0x39
#define SHT4X_MAX_HEAT_01  0x32
#define SHT4X_MED_HEAT_1   0x2F
#define SHT4X_MED_HEAT_01  0x24
#define SHT4X_LOW_HEAT_1   0x1E
#define SHT4X_LOW_HEAT_01  0x15


void idSHT4x(void){
    uint8_t buf[6] = {0};
    uint8_t reg = SHT4X_ID;
    i2c_write_blocking(i2c_default, SHT4X_ADDR, &reg, 1, false);
    sleep_ms(10);
    i2c_read_blocking(i2c_default, SHT4X_ADDR, buf, 6, false);
}

void readHighTH (struct SHT4x_meas *TH){
    uint8_t buf[6] = {0};
    uint8_t reg = SHT4X_MEAS_HIGH;
    i2c_write_blocking(i2c_default, SHT4X_ADDR, &reg, 1, false);
    sleep_ms(10);
    i2c_read_blocking(i2c_default, SHT4X_ADDR, buf, 6, false);
    sleep_ms(10);
    TH->rawT = buf[0] * 256 + buf[1];
    printf("rawT %d\n", TH->rawT);
    TH->rawH = buf[3] * 256 + buf[4];
    printf("rawH %d\n", TH->rawH);
    TH->T = -45.0 + 175.0 * (double)TH->rawT / ((double)(power(2, 16) - 1));
    TH->H = -6.0 + 125.0 * (double)TH->rawH / ((double)(power(2, 16) - 1));
}