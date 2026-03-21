#include "pico/stdlib.h"
#include "tusb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include "DPS310.h"
#include "SHT4x.h"
#include "BME680.h"
#include "cbor.h"
#include "friday.h"

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

size_t printBME680ParametersUSB(const bme680_temp_par_t * temp_par, const bme680_press_par_t * press_par, const bme680_hum_par_t * hum_par, const uint8_t * gas_sw_err){
    uint8_t msg[FRIDAY_MAX_PAYLOAD_SIZE];
    CborEncoder buf, bme680, temp, press, hum, gas;
    cbor_encoder_init(&buf, msg, sizeof(msg), 0);
    cbor_encoder_create_map(&buf, &bme680, 6);

    cbor_encode_text_stringz(&bme680, "sensor");
    cbor_encode_text_stringz(&bme680, "BME680");
    cbor_encode_text_stringz(&bme680, "type");
    cbor_encode_text_stringz(&bme680, "params");

    cbor_encode_text_stringz(&bme680, "temp");
    cbor_encoder_create_map(&bme680, &temp, 3);
    cbor_encode_text_stringz(&temp, "t1");
    cbor_encode_uint(&temp, temp_par->t1);
    cbor_encode_text_stringz(&temp, "t2");
    cbor_encode_int(&temp, temp_par->t2);
    cbor_encode_text_stringz(&temp, "t3");
    cbor_encode_int(&temp, temp_par->t3);
    cbor_encoder_close_container(&bme680, &temp);

    cbor_encode_text_stringz(&bme680, "press");
    cbor_encoder_create_map(&bme680, &press, 10);
    cbor_encode_text_stringz(&press, "p1");
    cbor_encode_uint(&press, press_par->p1);
    cbor_encode_text_stringz(&press, "p2");
    cbor_encode_int(&press, press_par->p2);
    cbor_encode_text_stringz(&press, "p3");
    cbor_encode_int(&press, press_par->p3);
    cbor_encode_text_stringz(&press, "p4");
    cbor_encode_int(&press, press_par->p4);
    cbor_encode_text_stringz(&press, "p5");
    cbor_encode_int(&press, press_par->p5);
    cbor_encode_text_stringz(&press, "p6");
    cbor_encode_int(&press, press_par->p6);
    cbor_encode_text_stringz(&press, "p7");
    cbor_encode_int(&press, press_par->p7);
    cbor_encode_text_stringz(&press, "p8");
    cbor_encode_int(&press, press_par->p8);
    cbor_encode_text_stringz(&press, "p9");
    cbor_encode_int(&press, press_par->p9);
    cbor_encode_text_stringz(&press, "p10");
    cbor_encode_uint(&press, press_par->p10);
    cbor_encoder_close_container(&bme680, &press);

    cbor_encode_text_stringz(&bme680, "hum");
    cbor_encoder_create_map(&bme680, &hum, 7);
    cbor_encode_text_stringz(&hum, "h1");
    cbor_encode_uint(&hum, hum_par->h1);
    cbor_encode_text_stringz(&hum, "h2");
    cbor_encode_uint(&hum, hum_par->h2);
    cbor_encode_text_stringz(&hum, "h3");
    cbor_encode_int(&hum, hum_par->h3);
    cbor_encode_text_stringz(&hum, "h4");
    cbor_encode_int(&hum, hum_par->h4);
    cbor_encode_text_stringz(&hum, "h5");
    cbor_encode_int(&hum, hum_par->h5);
    cbor_encode_text_stringz(&hum, "h6");
    cbor_encode_uint(&hum, hum_par->h6);
    cbor_encode_text_stringz(&hum, "h7");
    cbor_encode_int(&hum, hum_par->h7);
    cbor_encoder_close_container(&bme680, &hum);

    cbor_encode_text_stringz(&bme680, "gas");
    cbor_encoder_create_map(&bme680, &gas, 1);
    cbor_encode_text_stringz(&gas, "gas_sw_err");
    cbor_encode_uint(&gas, *gas_sw_err);
    cbor_encoder_close_container(&bme680, &gas);

    cbor_encoder_close_container(&buf, &bme680);
    size_t msg_len = cbor_encoder_get_buffer_size(&buf, msg);
    return sendFridayMessage(msg, msg_len);
}

size_t printBME680MeasureUSB(const uint8_t * temp, const uint8_t * press, const uint8_t * hum, const uint8_t * gas) {
    uint8_t msg[FRIDAY_MAX_PAYLOAD_SIZE];
    CborEncoder buf, bme680, temp_map, press_map, hum_map, gas_map;
    cbor_encoder_init(&buf, msg, sizeof(msg), 0);
    cbor_encoder_create_map(&buf, &bme680, 5);

    cbor_encode_text_stringz(&bme680, "sensor");
    cbor_encode_text_stringz(&bme680, "BME680");
    cbor_encode_text_stringz(&bme680, "type");
    cbor_encode_text_stringz(&bme680, "measure");

    cbor_encode_text_stringz(&bme680, "temp");
    cbor_encoder_create_map(&bme680, &temp_map, 3);
    cbor_encode_text_stringz(&temp_map, "t1");
    cbor_encode_uint(&temp_map, temp[0]);
    cbor_encode_text_stringz(&temp_map, "t2");
    cbor_encode_uint(&temp_map, temp[1]);
    cbor_encode_text_stringz(&temp_map, "t3");
    cbor_encode_uint(&temp_map, temp[2]);
    cbor_encoder_close_container(&bme680, &temp_map);

    cbor_encode_text_stringz(&bme680, "press");
    cbor_encoder_create_map(&bme680, &press_map, 3);
    cbor_encode_text_stringz(&press_map, "p1");
    cbor_encode_uint(&press_map, press[0]);
    cbor_encode_text_stringz(&press_map, "p2");
    cbor_encode_uint(&press_map, press[1]);
    cbor_encode_text_stringz(&press_map, "p3");
    cbor_encode_uint(&press_map, press[2]);
    cbor_encoder_close_container(&bme680, &press_map);

    cbor_encode_text_stringz(&bme680, "hum");
    cbor_encoder_create_map(&bme680, &hum_map, 2);
    cbor_encode_text_stringz(&hum_map, "h1");
    cbor_encode_uint(&hum_map, hum[0]);
    cbor_encode_text_stringz(&hum_map, "h2");
    cbor_encode_uint(&hum_map, hum[1]);
    cbor_encoder_close_container(&bme680, &hum_map);

    cbor_encode_text_stringz(&bme680,"gas");
    cbor_encoder_create_map(&bme680,&gas_map ,2);
	cbor_encode_text_stringz (&gas_map,"g1");
	cbor_encode_uint(&gas_map,gas[0]);
    cbor_encode_text_stringz (&gas_map,"g2");
    cbor_encode_uint(&gas_map,gas[1]);
	cbor_encoder_close_container (&bme680,&gas_map);

	cbor_encoder_close_container (&buf,&bme680);
	size_t msg_len = cbor_encoder_get_buffer_size (&buf,msg);
	return sendFridayMessage(msg,msg_len);
}

size_t printDPS310ParametersUSB(const uint8_t * coeff_buf){
    uint8_t msg[FRIDAY_MAX_PAYLOAD_SIZE];
    CborEncoder buf, dps310, coeffs;
    cbor_encoder_init(&buf, msg, sizeof(msg), 0);
    cbor_encoder_create_map(&buf, &dps310, 3);
    cbor_encode_text_stringz(&dps310, "sensor");
    cbor_encode_text_stringz(&dps310, "DPS310");
    cbor_encode_text_stringz(&dps310, "type");
    cbor_encode_text_stringz(&dps310, "params");

    cbor_encode_text_stringz(&dps310, "coeffs");
    cbor_encoder_create_map(&dps310, &coeffs, 18);
    for (int i = 0; i < 18; i++) {
        char key[4];
        snprintf(key, sizeof(key), "c%d", i);
        cbor_encode_text_stringz(&coeffs, key);
        cbor_encode_uint(&coeffs, coeff_buf[i]);
    }
    cbor_encoder_close_container(&dps310, &coeffs);
    cbor_encoder_close_container(&buf, &dps310);

    size_t msg_len = cbor_encoder_get_buffer_size(&buf, msg);
    return sendFridayMessage(msg, msg_len);
}

size_t printDPS310MeasureUSB(const uint8_t * temp_buf, const uint8_t * press_buf){
    uint8_t msg[FRIDAY_MAX_PAYLOAD_SIZE];
    CborEncoder buf, dps310, temp_map, press_map;
    cbor_encoder_init(&buf, msg, sizeof(msg), 0);
    cbor_encoder_create_map(&buf, &dps310, 4);

    cbor_encode_text_stringz(&dps310, "sensor");
    cbor_encode_text_stringz(&dps310, "DPS310");
    cbor_encode_text_stringz(&dps310, "type");
    cbor_encode_text_stringz(&dps310, "measure");

    cbor_encode_text_stringz(&dps310, "temp");
    cbor_encoder_create_map(&dps310, &temp_map, 3);
    cbor_encode_text_stringz(&temp_map, "t1");
    cbor_encode_uint(&temp_map, temp_buf[0]);
    cbor_encode_text_stringz(&temp_map, "t2");
    cbor_encode_uint(&temp_map, temp_buf[1]);
    cbor_encode_text_stringz(&temp_map, "t3");
    cbor_encode_uint(&temp_map, temp_buf[2]);
    cbor_encoder_close_container(&dps310, &temp_map);

    cbor_encode_text_stringz(&dps310, "press");
    cbor_encoder_create_map(&dps310, &press_map, 3);
    cbor_encode_text_stringz(&press_map, "p1");
    cbor_encode_uint(&press_map, press_buf[0]);
    cbor_encode_text_stringz(&press_map, "p2");
    cbor_encode_uint(&press_map, press_buf[1]);
    cbor_encode_text_stringz(&press_map, "p3");
    cbor_encode_uint(&press_map, press_buf[2]);
    cbor_encoder_close_container(&dps310, &press_map);

    cbor_encoder_close_container(&buf, &dps310);
    size_t msg_len = cbor_encoder_get_buffer_size(&buf, msg);
    return sendFridayMessage(msg, msg_len);
}

size_t printSHT4xMeasureUSB(const uint8_t * sh4x_buf){
    uint8_t msg[FRIDAY_MAX_PAYLOAD_SIZE];
    CborEncoder buf, sht4x, measure;
    cbor_encoder_init(&buf, msg, sizeof(msg), 0);
    cbor_encoder_create_map(&buf, &sht4x, 3);
    cbor_encode_text_stringz(&sht4x, "sensor");
    cbor_encode_text_stringz(&sht4x, "SHT4x");
    cbor_encode_text_stringz(&sht4x, "type");
    cbor_encode_text_stringz(&sht4x, "measure");

    cbor_encode_text_stringz(&sht4x, "data");
    cbor_encoder_create_map(&sht4x, &measure, 6);
    for (int i = 0; i < 6; i++) {
        char key[3];
        snprintf(key, sizeof(key), "b%d", i);
        cbor_encode_text_stringz(&measure, key);
        cbor_encode_uint(&measure, sh4x_buf[i]);
    }
    cbor_encoder_close_container(&sht4x, &measure);
    cbor_encoder_close_container(&buf, &sht4x);

    size_t msg_len = cbor_encoder_get_buffer_size(&buf, msg);
    return sendFridayMessage(msg, msg_len);
}

void usb_cdc_task(void *p) {
    (void) p;
    uint8_t rx_payload[FRIDAY_MAX_PAYLOAD_SIZE];
    rx_friday_t rx = { .state = RX_FRIDAY, .version = 0, .length = 0, .payload_idx = 0 };
    friday_payload_t payload;
    rx_friday_keys_t keys;
    uint8_t coeff_buf[18] = {0};
    size_t idx = 0;
    uint8_t b;
    gpio_init(LED_USB_PIN);
	gpio_set_dir(LED_USB_PIN, GPIO_OUT);
    dps310_t dps310 = { .initialized = false };
    bme680_t bme680 = { .initialized = false };

    while (1) {
        if (xQueueReceive(usb_rx_queue, &b, portMAX_DELAY) == pdTRUE) {
            receiveFridayMessage(&rx, b);
            if (rx.state == RX_COMPLETE) {
                rx.state = RX_FRIDAY;
                CborParser parser;
                CborValue it, map_it;
                size_t len;
                if (cbor_parser_init(rx.payload, rx.length, 0, &parser, &it) != CborNoError) {
                    continue;
                }

                if (!cbor_value_is_map(&it)) {
                    continue;
                }

                if (cbor_value_enter_container(&it, &map_it) != CborNoError) {
                    continue;
                }

                len = sizeof(keys.module);
                if (cbor_value_copy_text_string(&map_it, keys.module, &len, &map_it) != CborNoError) {
                    cbor_value_leave_container(&it, &map_it);
                    continue;    
                }

                if (strcmp(keys.module, "module") == 0) {
                    len = sizeof(payload.module);
                    if (cbor_value_copy_text_string(&map_it, payload.module, &len, &map_it) != CborNoError) {
                        cbor_value_leave_container(&it, &map_it);
                        continue;
                    }
                } else {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                len = sizeof(keys.name);
                if (cbor_value_copy_text_string(&map_it, keys.name, &len, &map_it) != CborNoError) {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                if (strcmp(keys.name, "name") == 0) {
                    len = sizeof(payload.name);
                    if (cbor_value_copy_text_string(&map_it, payload.name, &len, &map_it) != CborNoError) {
                        cbor_value_leave_container(&it, &map_it);
                        continue;
                    }
                } else {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                len = sizeof(keys.type);
                if (cbor_value_copy_text_string(&map_it, keys.type, &len, &map_it) != CborNoError) {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                if (strcmp(keys.type, "type") == 0) {
                    len = sizeof(payload.type);
                    if (cbor_value_copy_text_string(&map_it, payload.type, &len, &map_it) != CborNoError) {
                        cbor_value_leave_container(&it, &map_it);
                        continue;
                    }
                } else {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                len = sizeof(keys.value);
                if (cbor_value_copy_text_string(&map_it, keys.value, &len, &map_it) != CborNoError) {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                if (strcmp(keys.value, "value") == 0) {
                    if (cbor_value_get_int(&map_it, &payload.value) != CborNoError) {
                        cbor_value_leave_container(&it, &map_it);
                        continue;
                    }
                    cbor_value_advance(&map_it);
                } else {
                    cbor_value_leave_container(&it, &map_it);
                    continue;
                }

                if (strcmp(payload.module, "gpio") == 0) {
                    if (strcmp(payload.name, "led") == 0) {
                        if (strcmp(payload.type, "cmd") == 0) {
                            gpio_put(LED_USB_PIN, payload.value);
                            CborEncoder encoder, map;
                            uint8_t buf[FRIDAY_MAX_PAYLOAD_SIZE];
                            cbor_encoder_init(&encoder, buf, sizeof(buf), 0);
                            cbor_encoder_create_map(&encoder, &map, 3);
                            cbor_encode_text_stringz(&map, "type");
                            cbor_encode_text_stringz(&map, "status");
                            cbor_encode_text_stringz(&map, "gpio");
                            cbor_encode_text_stringz(&map, "led");
                            cbor_encode_text_stringz(&map, "value");
                            cbor_encode_uint(&map, payload.value);
                            cbor_encoder_close_container(&encoder, &map);
                            size_t len = cbor_encoder_get_buffer_size(&encoder, buf);
                            sendFridayMessage(buf, len);
                            continue;
                        }
                    }
                }

                if (strcmp(payload.module, "sensor") == 0) {
                    if (strcmp(payload.name, "dps310") == 0) {
                        if (strcmp(payload.type, "init") == 0) {
                            if (!dps310.initialized) {
                                if (configDPS310()) {
                                    dps310.initialized = true;
                                    continue;
                                }
                            }
                        } else if (strcmp(payload.type, "coeff") == 0) {
                            readRawCoeffDPS310(coeff_buf);
                            printDPS310ParametersUSB(coeff_buf);
                            continue; 
                        } else if (strcmp(payload.type, "meas") == 0) {
                            if (dps310.initialized) {
                                uint8_t temp_buf[3] = {0};
                                uint8_t press_buf[3] = {0};
                                dps310ReadTemp(temp_buf);
                                dps310ReadPress(press_buf);
                                printDPS310MeasureUSB(temp_buf, press_buf);
                                continue;
                            }
                        }
                    }

                    if (strcmp(payload.name, "sht4x") == 0) {
                        if (strcmp(payload.type, "meas") == 0) {
                            uint8_t sh4x_buf[6] = {0};
                            readHighTH(sh4x_buf);
                            printSHT4xMeasureUSB(sh4x_buf);
                            continue;
                        }
                    }

                    if (strcmp(payload.name, "bme680")==0){
                        if (strcmp(payload.type, "init") == 0) {
                            if (!bme680.initialized) {
                                bme680Configure();
                                bme680.initialized = true;
                                continue;
                            }
                        } else if (strcmp(payload.name, "calib") == 0) {
                            bme680_temp_par_t temp_par;
                            bme680_press_par_t press_par;
                            bme680_hum_par_t hum_par;
                            uint8_t gas_sw_err;
                            bme680GetCalibrationParameters(&temp_par, &press_par, &hum_par, &gas_sw_err);
                            printBME680ParametersUSB(&temp_par, &press_par, &hum_par, &gas_sw_err);
                            continue;
                        } else if (strcmp(payload.name, "meas") == 0) {
                            if (bme680.initialized) {
                                uint8_t temp_buf[3] = {0};
                                uint8_t press_buf[3] = {0};
                                uint8_t hum_buf[2] = {0};
                                uint8_t gas_buf[2] = {0};
                                bme680Measure(temp_buf, press_buf, hum_buf, gas_buf);
                                printBME680MeasureUSB(temp_buf, press_buf, hum_buf, gas_buf);
                                continue;
                            }
                        }
                    }
                }  
            }
        }  
    }
}

void cdc_init(void){
    usb_rx_queue = xQueueCreate(128, sizeof(uint8_t));
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