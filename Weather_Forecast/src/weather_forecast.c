#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>

#include <stdio.h>
#include "pico/stdlib.h"
#include "modules/led.h"


int main (void){
    xTaskCreate(blinkHandler, "blinker", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
	
	return 0;
}