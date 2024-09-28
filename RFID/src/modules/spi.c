#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

void spi_config () {
    // Setting SPI0 at 0.5MHz
    spi_init(spi_default, 500 * 1000);
    
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so it is initialised to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));
    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void cs_select() {
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);
}

void cs_deselect(){
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
}

void spi_read(uint8_t * reg, uint8_t * buf, uint16_t len) {
    cs_select();
    spi_write_blocking(spi_default, reg, 1);
    sleep_ms(10);
    spi_read_blocking(spi_default, 0, buf, len);
    cs_deselect();
}

void spi_write(uint8_t reg, uint8_t * data) {
    uint8_t length = sizeof(data)/sizeof(data[0]);
    uint8_t * buf;
    buf[0] = reg;
    for(int i = 0; i < length; i++){
        buf[i+1] = data[i];
    }
    cs_select();
    spi_write_blocking(spi_default, buf, length+1);
    sleep_ms(10);
    cs_deselect();
}