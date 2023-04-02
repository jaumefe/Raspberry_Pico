#include <stdio.h>
#include "DPS310.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"



//Register for temperature and humidty sensor (SHT)
#define SHT4XA_ADDR         0x44
#define SHT4XA_MEAS         0xFD

//I2C reserves some addresses for special purposes.
    //These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr){
        return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
    }



int main (){
    struct DPS310_coeff params;
    //Enable UART so status output can be printed
    stdio_init_all();
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

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

    uint8_t buf[2] = {0};
    uint8_t reg = DPS310_ID;
     

    sleep_ms(2000);
    i2c_write_blocking(i2c_default, DPS310_ADDR, &reg, 1, false);
    i2c_read_blocking(i2c_default, DPS310_ADDR, buf, 2, false);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);
        
    

    gpio_put(LED_PIN, 1);
    
    sleep_ms(1000);

    readCoeffDPS310(&params);
    
    printf("%c \n", buf[0]);
    printf("%c \n", buf[1]);
    gpio_put(LED_PIN, 0);

    sleep_ms(1000);
    
    return 0;
#endif
}