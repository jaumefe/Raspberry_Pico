#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>

#include "hardware/gpio.h"

const uint LED_PIN = 0;
const TickType_t ms_delay = 1000 / portTICK_PERIOD_MS;

void blinkHandler(void *p){
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