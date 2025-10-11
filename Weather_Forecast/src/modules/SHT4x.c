// Modelo de chip utilizado SHT40-AD1B-R2: https://www.mouser.es/datasheet/2/682/Datasheet_SHT4x-3003109.pdf
#include <stdio.h>
#include "SHT4x.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"


void idSHT4x(void){
    uint8_t buf[6] = {0};
    uint8_t reg = SHT4X_ID;
    i2c_write_blocking(i2c_default, SHT4X_ADDR, &reg, 1, false);
    sleep_ms(10);
    i2c_read_blocking(i2c_default, SHT4X_ADDR, buf, 6, false);
}

void readHighTH (uint8_t * buf){
    uint8_t reg = SHT4X_MEAS_HIGH;
    i2c_write_blocking(i2c_default, SHT4X_ADDR, &reg, 1, true);
    sleep_ms(10);
    i2c_read_blocking(i2c_default, SHT4X_ADDR, buf, 6, false);
    sleep_ms(10);
}