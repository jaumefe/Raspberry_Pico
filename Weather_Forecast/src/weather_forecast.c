#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>

#include <stdio.h>
#include "pico/stdlib.h"
#include "modules/led.h"
#include "core/usb_cdc.h"



int main (void){
	stdio_init_all();
	usb_cdc_init();
    xTaskCreate(blinkHandler, "blinker", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
	
	return 0;
}