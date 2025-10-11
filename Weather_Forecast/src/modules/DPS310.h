#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef DPS310_H
#define DPS310_H

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

typedef struct dps310_s {
    bool initialized;
} dps310_t;

void readRawCoeffDPS310 (uint8_t * buf);

bool configDPS310(void);

void prepareMeasDPS310(void);

void dps310ReadTemp(uint8_t * buf);

void dps310ReadPress(uint8_t * buf);

uint8_t idDPS310(void);

void correctTemp(void);
#endif //DPS310_H