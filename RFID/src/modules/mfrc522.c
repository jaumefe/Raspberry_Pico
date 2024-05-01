#include <stdio.h>
#include "pico/stdlib.h"
#include "mfrc522.h"
#include "spi.h"

uint8_t * selfTest () {
    // Perform a soft reset
    uint8_t cmd = SOFT_RESET | 0x2F;
    uint8_t reg = COMMAND_REG;
    spi_write(reg, cmd);

    // Clear internal buffer by writing 25 bytes of 00h and implement the Config command
    cmd = 0x00;
    reg = FIFO_DATA_REG;
    for(int i = 0; i < 25; i++) {
        spi_write(reg, cmd);
    }

    // Enable the self test by wirting 09h to AutoTestReg register
    cmd = 0x09;
    reg = AUTO_TEST_REG;
    spi_write(reg, cmd);

    // Write 00h to the FIFO buffer
    cmd = 0x00;
    reg = FIFO_DATA_REG;
    spi_write(reg, cmd);

    // Start the self test with the CalcCRC command
    cmd = CALC_CRC | 0x2F;
    reg = COMMAND_REG;
    spi_write(reg, cmd);

    // The self test is initiated

    // When completed, the FIFO buffer contains 64 bytes
    reg = FIFO_DATA_REG;
    uint16_t len = 64;
    uint8_t * buf;
    spi_read(reg, buf, len);

    return buf;
}