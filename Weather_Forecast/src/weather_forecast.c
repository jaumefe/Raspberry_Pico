#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "modules/led.h"
#include "modules/usb_cdc.h"



int main (void){
	stdio_init_all();
	usb_cdc_init();
	i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    xTaskCreate(blinkHandler, "blinker", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
	
	return 0;
}