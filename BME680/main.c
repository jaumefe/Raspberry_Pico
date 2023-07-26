#include <stdio.h>
#include "BME680.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"


bool reserved_addr(uint8_t addr){
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main(){
    struct BME680_par par;
    struct BME680_meas meas;
    stdio_init_all();

#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c / bmp280_i2c example requires a board with I2C pins
        puts("Default I2C pins were not defined");
#else
    //I2C initialization: Configured at 100kHz et on default pins
    i2c_init(i2c_default, 100*1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    sleep_ms(2000);
    // Setting the variables for BME680    
    measureBME680(&par, 300, 25, 0);

    // Main Loop
    while (1){
        forcedMode();
        sleep_ms(250);
        tempBME680(&par, &meas);
        humidityBME680(&par, &meas);
        pressBME680(&par, &meas);
        gasResBME680(&par, &meas);
        printf("Temperatura (ºC): %f\n", meas.temp_comp);
        printf("Humitat: %f\n", meas.hum_comp);
        printf("Pressió (Pa): %f\n", meas.press_comp);
        printf("Resistència Gas (Ohm): %f\n", meas.gas_res);
        sleep_ms(30*60*1000);
    }
    return 0;
#endif
}