#include <stdio.h>
#include "DPS310.h"
#include "SHT4x.h"
#include "basic.h"
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

int main (void){
    struct DPS310_coeff params;
    struct DPS310_meas meas;
    struct SHT4x_meas TH;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    int led = 0;
    uint8_t ID;
    //Enable UART so status output can be printed
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, led);

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
    // LED blinking
    led = blinkLED(led, LED_PIN);
    printf("punt1\n");

    sleep_ms(5000);
    // Reading ID of the DPS310 (and LED blink for debug)
    //idSHT4x();
    readHighTH(&TH);
    printf("T: %f \n", TH.T);
    printf("H: %f \n", TH.H);
    //ID = idDPS310();
    //printf("ID: %d \n", ID);    
    led = blinkLED(led, LED_PIN);
    printf("punt2\n");
    sleep_ms(2000);

    // Reading coefficients for compensation (and LED blink for debug)
    readCoeffDPS310(&params);
    led = blinkLED(led, LED_PIN);
    printf("punt3\n");

    // Configuring the sensor and starting it
    configDPS310();
    led = blinkLED(led, LED_PIN);
    printf("punt4\n");
    sleep_ms(2000);

    meas.T = TH.T;    
    
    // Pressure measurement (Temperature measurement included for compensation)
    readPress(&meas, &params);
    printf("punt5\n");
    led = blinkLED(led, LED_PIN);
    printf("punt6\n");
    sleep_ms(3000);
    

    // Printing result
    led = blinkLED(led, LED_PIN);
    printf("END");
    
    return 0;
#endif
}