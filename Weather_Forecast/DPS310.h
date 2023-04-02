#include <stdio.h>
#include "pico/stdlib.h"

#ifndef DPS310_H
#define DPS310_H
    struct DPS310_coeff;
    void readCoeffDPS310(struct DPS310_coeff * params);
    void startupDPS310();
#endif //DPS310_H