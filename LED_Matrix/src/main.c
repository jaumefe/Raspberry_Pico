#include "modules/led_matrix_i2c.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/binary_info.h"
#include "hardware/timer.h"

//I2C reserves some addresses for special purposes.
    //These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr){
        return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main (void) {
    stdio_init_all();
    #if !defined (i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
        #warning i2c/bus_scan example requires a board with I2C pins
        puts("Default I2C pins were not defined");
    #else
    // In this case we will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    sleep_ms(5000);

    init();

    const RGBLookup_t colors[4] = {
        {255, 0, 0},
        {0, 255, 0},
        {0, 0, 255},
        {128, 128, 128}
    };

    uint8_t col = 0;
    while(1) {
        RGBLookup_t color = colors[col];
        for (auto x = 0u; x < 5; x++){
            for (auto y = 0u; y < 5; y++){
                set_pixel(x, y, color.r, color.g, color.b);
            }
        }
        IS31FL3731_update(0);
        sleep_ms(1000);
        col++;
        col %= 4;
    }
    return 0;
    #endif
}