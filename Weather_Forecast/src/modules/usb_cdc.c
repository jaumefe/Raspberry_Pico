#include "pico/stdlib.h"
#include "tusb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include "DPS310.h"
#include "SHT4x.h"
#include "BME680.h"

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

void printBufToUSB(const uint8_t * buf, uint8_t size){
    for (int i = 0; i < size; i++) {
        char byte_str[5];
        snprintf(byte_str, sizeof(byte_str), "%02X ", buf[i]);
        tud_cdc_write_str(byte_str);
    }
}

void printBME680ParametersUSB(const bme680_temp_par_t * temp_par, const bme680_press_par_t * press_par, const bme680_hum_par_t * hum_par){
    char param_str[256];
    snprintf(param_str, sizeof(param_str), "Temp Par: %u, %d, %d\r\n", temp_par->t1, temp_par->t2, temp_par->t3);
    tud_cdc_write_str(param_str);
    tud_cdc_write_flush();
    snprintf(param_str, sizeof(param_str), "Press Par: %u, %d, %d, %d, %d, %d, %d, %d, %d\r\n",
             press_par->p1, press_par->p2, press_par->p3, press_par->p4,
             press_par->p5, press_par->p6, press_par->p7, press_par->p8,
             press_par->p9);
    tud_cdc_write_str(param_str);
    tud_cdc_write_str("\r\n");
    tud_cdc_write_flush();
    snprintf(param_str, sizeof(param_str), "Hum Par: %u, %u, %d, %d, %d, %d, %d\r\n",
             hum_par->h1, hum_par->h2, hum_par->h3,
             hum_par->h4, hum_par->h5, hum_par->h6, hum_par->h7);
    tud_cdc_write_str(param_str);
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
    bme680_t bme680 = { .initialized = false };

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
                    readRawCoeffDPS310(coeff_buf);
                    tud_cdc_write_str("DPS310 Coefficients:\r\n");
                    printBufToUSB(coeff_buf, sizeof(coeff_buf));
                    tud_cdc_write_str("\r\n");
                } else if (strcmp(cmd_buf, "DPS310_INIT") == 0) {
                    if (!dps310.initialized) {
                        if(configDPS310()){
                            dps310.initialized = true;
                            tud_cdc_write_str("DPS310 Initialized\r\n");
                        } else {
                            tud_cdc_write_str("DPS310 Initialization Failed\r\n");
                        }                    
                    } else {
                        tud_cdc_write_str("DPS310 was already initialized\r\n");
                    }
                } else if (strcmp(cmd_buf, "DPS310_MEAS") == 0) {
                    if (dps310.initialized) {
                        uint8_t temp_buf[3] = {0};
                        dps310ReadTemp(temp_buf);
                        tud_cdc_write_str("DPS310 Temperature Raw Data:\r\n");
                        printBufToUSB(temp_buf, sizeof(temp_buf));
                        tud_cdc_write_str("\r\n");
                        tud_cdc_write_flush();

                        uint8_t press_buf[3] = {0};
                        dps310ReadPress(press_buf);
                        tud_cdc_write_str("DPS310 Pressure Raw Data:\r\n");
                        printBufToUSB(press_buf, sizeof(press_buf));
                        tud_cdc_write_str("\r\n");
                    } else {
                        tud_cdc_write_str("DPS310 is not initialized\r\n");
                    }
                } else if (strcmp(cmd_buf, "SHT4x_MEAS") == 0) {
                    uint8_t sh4x_buf[6] = {0};
                    readHighTH(sh4x_buf);
                    tud_cdc_write_str("SHT4x Raw Data:\r\n");
                        for (int i = 0; i < 6; i++) {
                            char byte_str[5];
                            snprintf(byte_str, sizeof(byte_str), "%02X ", sh4x_buf[i]);
                            tud_cdc_write_str(byte_str);
                        }
                        tud_cdc_write_str("\r\n");
                } else if (strcmp(cmd_buf, "BME680_CALIB") == 0) {
                    bme680_temp_par_t temp_par;
                    bme680_press_par_t press_par;
                    bme680_hum_par_t hum_par;
                    bme680GetCalibrationParameters(&temp_par, &press_par, &hum_par);
                    tud_cdc_write_str("BME680 Calibration Parameters:\r\n");
                    printBME680ParametersUSB(&temp_par, &press_par, &hum_par);
                    tud_cdc_write_str("\r\n");
                } else if (strcmp(cmd_buf, "BME680_CONFIG") == 0) {
                    if (!bme680.initialized) {
                        bme680Configure();
                        bme680.initialized = true;
                        tud_cdc_write_str("BME680 Configured\r\n");
                    } else {
                        tud_cdc_write_str("BME680 was already initialized\r\n");
                    }
                } else if (strcmp(cmd_buf, "BME680_MEAS") == 0) {
                    if (bme680.initialized) {
                        uint8_t temp_buf[3] = {0};
                        uint8_t press_buf[3] = {0};
                        uint8_t hum_buf[2] = {0};
                        uint8_t gas_buf[2] = {0};
                        bme680Measure(temp_buf, press_buf, hum_buf, gas_buf);

                        tud_cdc_write_str("BME680 Temperature Raw Data:\r\n");
                        printBufToUSB(temp_buf, sizeof(temp_buf));
                        tud_cdc_write_str("\r\n");
                        tud_cdc_write_flush();

                        tud_cdc_write_str("BME680 Pressure Raw Data:\r\n");
                        printBufToUSB(press_buf, sizeof(press_buf));
                        tud_cdc_write_str("\r\n");
                        tud_cdc_write_flush();

                        tud_cdc_write_str("BME680 Humidity Raw Data:\r\n");
                        printBufToUSB(hum_buf, sizeof(hum_buf));
                        tud_cdc_write_str("\r\n");
                        tud_cdc_write_flush();

                        tud_cdc_write_str("BME680 Gas Resistance Raw Data:\r\n");
                        printBufToUSB(gas_buf, sizeof(gas_buf));
                        tud_cdc_write_str("\r\n");
                        tud_cdc_write_flush();
                    } else {
                        tud_cdc_write_str("BME680 is not initialized\r\n");
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