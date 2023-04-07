#include <stdio.h>
#include "pico/stdlib.h"

#ifndef DPS310_H
#define DPS310_H
    struct DPS310_coeff;
    struct DPS310_meas;
    void readCoeffDPS310(struct DPS310_coeff * params);
    void startupDPS310();
    void readPress(struct DPS310_meas * meas, struct DPS310_coeff * params);
    uint8_t idDPS310();
#endif //DPS310_H