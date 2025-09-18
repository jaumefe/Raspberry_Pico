#include <stdio.h>
#include "basic.h"
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

typedef struct DPS310_coeff_s{
    int16_t c0, c1, c01, c11, c20, c21, c30;
    int32_t c00, c10;
} DPS310_coeff_t;
    
typedef struct DPS310_meas_s{
    int32_t rawT, rawP;
    double T, P;
} DPS310_meas_t;

void readCoeffDPS310 (DPS310_coeff_t * params);

void configPress(void);

void configTemp(void);

void configInt(void);

void configDPS310(void);

void startupDPS310(void);

void readTemp(DPS310_meas_t * meas, DPS310_coeff_t * params);

void readPress(DPS310_meas_t * meas, DPS310_coeff_t * params);

uint8_t idDPS310(void);

void correctTemp(void);
#endif //DPS310_H