#ifndef _SPI_H
#define _SPI_H

void spi_config();
void spi_read(uint8_t reg, uint8_t * buf, uint16_t len);
void spi_write(uint8_t reg, uint8_t data);

#endif