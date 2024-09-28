#include <stdio.h>
#include "pico/stdlib.h"
#include "modules/spi.h"
#include "modules/mfrc522.h"

int main(){
    stdio_init_all();
    printf("Configuring\n");
    sleep_ms(5000);
    spi_config();
    uint8_t buf[64] = {0};
    printf("Entering self test mode\n");
    selfTest(buf);
    while(1){
        for (int i = 0; i < 64; i++){
            printf("%d ", buf[i]);
        }
        sleep_ms(5000);
        printf("\n");
    }
}