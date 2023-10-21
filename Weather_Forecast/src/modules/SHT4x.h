#include <stdio.h>
#include "basic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#ifndef SHT4x_H
#define SHT4x_H

struct SHT4x_meas{
        int32_t rawT, rawH;
        double T, H;
    };

void idSHT4x(void);
void readHighTH (struct SHT4x_meas * TH);
#endif //SHT4x_H