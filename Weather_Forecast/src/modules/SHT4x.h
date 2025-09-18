#include <stdio.h>
#include "basic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef SHT4x_H
#define SHT4x_H

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

typedef struct SHT4x_meas_s{
        int32_t rawT, rawH;
        double T, H;
} SHT4x_meas_t;

void idSHT4x(void);
void readHighTH (SHT4x_meas_t * TH);
#endif //SHT4x_H