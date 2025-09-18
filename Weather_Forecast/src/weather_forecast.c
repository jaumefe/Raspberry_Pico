#include <stdio.h>
#include "DPS310.h"
#include "SHT4x.h"
#include "basic.h"
#include "BME680.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"


int main (void){
    DPS310_coeff_t params;
    DPS310_meas_t meas;
    SHT4x_meas_t TH;
    BME680_par_t par;
    BME680_meas_t mes;
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

    // LED blinking
    led = blinkLED(led, LED_PIN);

    sleep_ms(5000);
    // Reading coefficients for compensation for DPS310 (and LED blink for debug)
    readCoeffDPS310(&params);

    // Configuring DPS310 sensor
    configDPS310();

    // Configuring BME680 sensor
    measureBME680(&par, 300, 25, 0);

    // LED blinking
    led = blinkLED(led, LED_PIN);
    sleep_ms(500);

    // Main loop
    while (1) {
        // LED blinking
        led = blinkLED(led, LED_PIN);

        // Temperature measure
        readHighTH(&TH);
        printf("Temperatura (ºC): %f \n", TH.T);
        printf("Humitat (%%): %f \n", TH.H);
        sleep_ms(100);
        meas.T = TH.T;

        // Pressure measurement (Temperature measurement included for compensation)
        startupDPS310();
        readPress(&meas, &params);
        printf("Pressió (Pa): %f\n", meas.P);

        led = blinkLED(led, LED_PIN);
        forcedMode();
        sleep_ms(250);
        tempBME680(&par, &mes);
        humidityBME680(&par, &mes);
        pressBME680(&par, &mes);
        gasResBME680(&par, &mes);
        printf("Temperatura (ºC): %f\n", mes.temp_comp);
        printf("Humitat (%%): %f\n", mes.hum_comp);
        printf("Pressió (Pa): %f\n", mes.press_comp);
        printf("Resistència_Gas (Ohm): %f\n", mes.gas_res);
        
        // Waiting for the next measurement in 30 minutes. It has been substrated the operation time
        sleep_ms(30*60*1000 - 1520);
        
    }
    
    return 0;
#endif
}