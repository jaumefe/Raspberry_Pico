#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#define LCD_ADDR        0x4E

bool reserved_addr(uint8_t addr){
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main(){
    stdio_init_all();
    uint8_t reg[] = {0b00100011, 0b00100000};
    #if !defined (i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
        #warning i2c/bus_scan example requires a board with I2C pins
        puts("Default I2C pins were not defined");
    #else
        // In this case we will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
        i2c_init(i2c_default, 100 * 1000);
        gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
        gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
        // Make the I2C pins available to picotool
        bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
        sleep_ms(5000);
        i2c_write_blocking(i2c_default, LCD_ADDR, &reg, 2, false);
        printf("Command sent\n");
        
        return 0;
    #endif
}