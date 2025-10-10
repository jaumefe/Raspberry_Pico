#include "pico/stdlib.h"
#include "tusb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include "DPS310.h"

const uint LED_USB_PIN = 1;

static QueueHandle_t usb_rx_queue = NULL;

#define USB_RX_QUEUE_SIZE 1024
static char rx_buffer[USB_RX_QUEUE_SIZE];

void usb_task(void *param) {
    (void) param;
    while (1) {
        tud_task();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void usb_init(void){
    tusb_init();
    stdio_usb_init();

    xTaskCreate(usb_task, "usb_task", 1024, NULL, configMAX_PRIORITIES - 1, NULL);
    return;
}

void tud_cdc_rx_cb(uint8_t itf) {
    (void) itf;
    while (tud_cdc_available()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        size_t count = tud_cdc_read(rx_buffer, sizeof(rx_buffer));
        if (usb_rx_queue != NULL) {
            for (size_t i = 0; i < count; i++) {
                xQueueSendFromISR(usb_rx_queue, &rx_buffer[i], &xHigherPriorityTaskWoken);
            }
        }
    }
}

void usb_cdc_task(void *p) {
    (void) p;
    char cmd_buf[128]= {0};
    uint8_t coeff_buf[18] = {0};
    size_t idx = 0;
    char c;
    gpio_init(LED_USB_PIN);
	gpio_set_dir(LED_USB_PIN, GPIO_OUT);
    dps310_t dps310 = { .initialized = false };

    while (1) {
        if (xQueueReceive(usb_rx_queue, &c, portMAX_DELAY) == pdTRUE) {
            if (c != '\n' && c != '\r') {
                if (idx < sizeof(cmd_buf) - 1) {
                    cmd_buf[idx++] = c;
                } else {
                    idx = 0;
                    cmd_buf[0] = '\0';
                }
            } else if (c == '\n') {
                cmd_buf[idx] = '\0';
                if (strcmp(cmd_buf, "LED_ON") == 0) {
                    gpio_put(LED_USB_PIN, 1);
                    tud_cdc_write_str("LED turned ON\r\n");
                } else if (strcmp(cmd_buf, "LED_OFF") == 0) {
                    gpio_put(LED_USB_PIN, 0);
                    tud_cdc_write_str("LED turned OFF\r\n");
                } else if (strcmp(cmd_buf, "DPS310_COEFF") == 0) {
                    readCoeffDPS310(coeff_buf);
                    tud_cdc_write_str("DPS310 Coefficients:\r\n");
                    for (int i = 0; i < 18; i++) {
                        char byte_str[5];
                        snprintf(byte_str, sizeof(byte_str), "%02X ", coeff_buf[i]);
                        tud_cdc_write_str(byte_str);
                    }
                    tud_cdc_write_str("\r\n");
                } else if (strcmp(cmd_buf, "DPS310_INIT") == 0) {
                    if (!dps310.initialized) {
                        dps310.initialized = true;
                        configDPS310();                    
                        tud_cdc_write_str("DPS310 Initialized\r\n");
                    } else {
                        tud_cdc_write_str("DPS310 was already initialized\r\n");
                    }
                } else {
                    tud_cdc_write_str("Unknown command: ");
                    tud_cdc_write_str(cmd_buf);
                    tud_cdc_write_str("\r\n");
                }
                tud_cdc_write_flush();

                idx = 0;
                cmd_buf[0] = '\0';
            }
        }  
    }
}

void cdc_init(void){
    usb_rx_queue = xQueueCreate(128, sizeof(char));
    if (usb_rx_queue == NULL) {
        while (1);
    }
    xTaskCreate(usb_cdc_task, "usb_cdc_task", 1024, NULL, tskIDLE_PRIORITY+1, NULL);
    return;
}

void usb_cdc_init(void){
    usb_init();
    cdc_init();

    return;
}