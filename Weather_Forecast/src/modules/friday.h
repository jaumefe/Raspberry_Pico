#ifndef FRIDAY_H
#define FRIDAY_H

#define FRIDAY_HEADER 0x46 // 'F' in ASCII
#define FRIDAY_VERSION 0x01
#define FRIDAY_MAX_PAYLOAD_SIZE 512

typedef enum {
    RX_FRIDAY,
    RX_VERSION,
    RX_LEN1,
    RX_LEN2,
    RX_PAYLOAD
} rx_friday_state_t;

typedef struct rx_friday_s{
    rx_friday_state_t state;
    uint8_t version;
    uint16_t length;
    uint8_t payload[FRIDAY_MAX_PAYLOAD_SIZE];
    uint16_t payload_idx;
} rx_friday_t;

size_t sendFridayMessage(const uint8_t * msg, size_t len);
void receiveFridayMessage(rx_friday_t * rx, uint8_t byte);

#endif //FRIDAY_H