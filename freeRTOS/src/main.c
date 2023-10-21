// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>

#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

const uint LED_PIN = 2;
const uint LED_PIN1 = 1;
const TickType_t ms_delay = 500 / portTICK_PERIOD_MS;
const TickType_t ms_delay1 = 500 / portTICK_PERIOD_MS;
volatile uint32_t ulIdleCycleCount = 0UL;

void blinkFn(void *p) {
	(void) p; // unused parameter
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	while (1) {
		gpio_put(LED_PIN, 0);
		vTaskDelay(ms_delay);
		gpio_put(LED_PIN, 1);
		vTaskDelay(ms_delay);
	}
}

void blinkFn1(void *p) {
	(void) p; // unused parameter
	gpio_init(LED_PIN1);
	gpio_set_dir(LED_PIN1, GPIO_OUT);

	while (1) {
		gpio_put(LED_PIN1, 1);
		vTaskDelay(ms_delay1);
		gpio_put(LED_PIN1, 0);
		vTaskDelay(ms_delay1);
	}
}

int main(void) {
	xTaskCreate(blinkFn, "blinker", 1024, NULL, 1, NULL);
    xTaskCreate(blinkFn1, "blinker1", 1024, NULL, 1, NULL);
	vTaskStartScheduler();
	
	return 0;
}