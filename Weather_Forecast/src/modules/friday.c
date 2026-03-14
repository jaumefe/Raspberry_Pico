#include <stdio.h>
#include "pico/stdlib.h"
#include "friday.h"
#include "tusb.h"

/*
Helper functions for USB communication protocol (friday)
*/

size_t sendFridayMessage(const uint8_t * msg, size_t len) {
    // Add header, version and length to the message (4 bytes)
    size_t total_len = len + 4;
    uint8_t buf[total_len];
    buf[0] = FRIDAY_HEADER;
    buf[1] = FRIDAY_VERSION;
    buf[2] = (len >> 8) & 0xFF;
    buf[3] = len & 0xFF;
    for (size_t i = 0; i < len; i++) {
        buf[i+4] = msg[i];
    }
    
    // Send the message
    size_t written = 0;
    while (written < total_len) {
        size_t n = tud_cdc_write(buf+written, total_len-written);
        if (n == 0) {
            tud_cdc_write_flush();
        } else {
            written += n;
        }
    }
    return written;
}

static void rx_reset(rx_friday_t *rx) {
    rx->state = RX_FRIDAY;
    rx->version = 0;
    rx->length = 0;
    memset(rx->payload, 0, FRIDAY_MAX_PAYLOAD_SIZE);
    rx->payload_idx = 0;
}

void receiveFridayMessage(rx_friday_t * rx, uint8_t byte) {
    switch (rx->state) {
        case RX_FRIDAY:
            if (byte == FRIDAY_HEADER) {
                rx->state = RX_VERSION;
                memset(rx->payload, 0, FRIDAY_MAX_PAYLOAD_SIZE);
            }
            break;
        case RX_VERSION:
            if (byte == FRIDAY_VERSION) {
                rx->version = byte;
                rx->state = RX_LEN1;
            } else {
                rx_reset(rx);
            }
            break;
        case RX_LEN1:
            rx->length = byte << 8;
            rx->state = RX_LEN2;
            break;
        case RX_LEN2:
            rx->length |= byte;
            if (rx->length > FRIDAY_MAX_PAYLOAD_SIZE) {
                // Invalid length, reset state
                rx_reset(rx);
            } else {
                rx->payload_idx = 0;
                rx->state = RX_PAYLOAD;
            }
            break;
        case RX_PAYLOAD:
            rx->payload[rx->payload_idx++] = byte;
            if (rx->payload_idx >= rx->length) {
                // Message received, process it
                rx->state = RX_COMPLETE;
            }
    }
}