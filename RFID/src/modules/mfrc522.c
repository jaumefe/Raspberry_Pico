#include <stdio.h>
#include "pico/stdlib.h"
#include "mfrc522.h"
#include "spi.h"

void selfTest (uint8_t * buf) {
    // Perform a soft reset
    uint8_t cmd[1] = {0};
    uint8_t reg = COMMAND_REG;
    cmd[0] = SOFT_RESET;
    printf("Soft reset\n");
    mfrc522_write(reg, cmd);
    sleep_ms(150);

    // Clear internal buffer by writing 25 bytes of 00h and implement the Config command
    cmd[0] = 0x80;
    reg = FIFO_LEVEL_REG;
    mfrc522_write(reg, cmd);
    
    uint8_t zeros[25] = {0x00};
    reg = FIFO_DATA_REG;
    mfrc522_write(reg, zeros);

    cmd[0] = MEM;
    reg = COMMAND_REG;
    mfrc522_write(reg, cmd);

    // Enable the self test by wirting 09h to AutoTestReg register
    cmd[0] = 0x09;
    reg = AUTO_TEST_REG;
    mfrc522_write(reg, cmd);

    // Write 00h to the FIFO buffer
    cmd[0] = 0x00;
    reg = FIFO_DATA_REG;
    mfrc522_write(reg, cmd);

    // Start the self test with the CalcCRC command
    cmd[0] = CALC_CRC;
    reg = COMMAND_REG;
    mfrc522_write(reg, cmd);

    // The self test is initiated

    // When completed, the FIFO buffer contains 64 bytes
    uint8_t readReg[1] = {FIFO_DATA_REG};
    mfrc522_read(readReg, buf);
}

void mfrc522_write(uint8_t reg, uint8_t * data){
    uint8_t address = (reg << 1) & 0x7E;
    printf("Address to write: %d\n", address);
    spi_write(address, data);
}

void mfrc522_read(uint8_t * reg, uint8_t * buf){
    uint8_t reg_len = sizeof(reg)/sizeof(reg[0]);
    uint8_t * address;
    for(int i = 0; i < reg_len; i++){
        address[i] = ((reg[i] << 1) | 0x80) & 0xFE;
    }
    address[reg_len] = 0x00;
    spi_read(address, buf, reg_len+1);
}