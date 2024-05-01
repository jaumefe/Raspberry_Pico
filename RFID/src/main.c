#include <stdio.h>
#include "pico/stdlib.h"
#include "modules/spi.h"
#include "modules/mfrc522.h"

int main(){
    stdio_init_all();
    spi_config();
    uint8_t * buf;
    buf = selfTest();
    while(1){
        for (int i = 0; i < 64; i++){
            printf("%d ", &buf[i]);
        }
        sleep_ms(1000);
        printf("\n");
    }
}