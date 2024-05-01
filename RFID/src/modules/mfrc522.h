/*MFRC522 registers*/
// Command and status
#define COMMAND_REG         0x01 // Start and stop command execution
#define COM_IEN_REG         0x02 // Enable and disable interrupt request control bits
#define DIV_IEN_REG         0x03 // Enable and disable interrupt request control bits
#define COM_IRQ_REG         0x04 // Interrupt request bits
#define DIV_IRQ_REG         0x05 // Interrupt request bits
#define ERROR_REG           0x06 // Error bits showing the error status of the last command executed
#define STATUS1_REG         0x07 // Communication status bits
#define STATUS2_REG         0x08 // Receiver and transmitter status bits
#define FIFO_DATA_REG       0x09 // Input and output of 64 byte FIFO buffer
#define FIFO_LEVEL_REG      0x0A // Number of bytes stored in the FIFO buffer
#define WATER_LEVEL_REG     0x0B // Level for FIFO underflow and overflow warning
#define CONTROL_REG         0x0C // Miscellaneous control registers
#define Bit_FRAMING_REG     0x0D // Adjustments for bit-oriented frames
#define COLL_REG            0x0E // Bit position of the first bit-collision detected on the RF interface

// Command
#define MODE_REG            0x11 // Defines general modes of transmitting and receiving
#define TXMODE_REG          0x12 // Defines transmission data rate and framing
#define RXMODE_REG          0x13 // Defines reception data rate and framing
#define TXCONTROL_REG       0x14 // Controls the logical behaviour of the antenna driver pins TX1 and TX2
#define TXASK_REG           0x15 // Controls the setting of the transmission modulation
#define TXSEL_REG           0x16 // Selects the internal sources for the antenna driver
#define RXSEL_REG           0x17 // Selects internal receiver settings
#define RXTHRESHOLD_REG     0x18 // Selects threshold for the bit decoder
#define DEMOD_REG           0x19 // Defines demodulator settings
#define MFTX_REG            0x1C // Controls some MIFARE communication transmit parameters
#define MFRX_REG            0x1D // Controls some MIFARE communication receive parameters
#define SERIALSPEED_REG     0x1F // Selects the speed of the serial UART interface

// Configuration
#define CRC_MSB_REG         0x21 // Shows the MSB values of the CRC calculation
#define CRC_LSB_REG         0x22 // Shows the LSB values of the CRC calculation
#define MOD_WIDTH_REG       0x24 // Controls the ModWidth setting
#define RF_CFG_REG          0x26 // Configures the receiver gain
#define GSN_REG             0x27 // Selects the conductance of the antenna driver pins TX1 and TX2 for modulation
#define CWGSP_REG           0x28 // Defines the conductance of the p-driver output during periods of no modulation
#define MODGSP_REG          0x29 // Defines the conductance of the p-drier output during periods of modulation
#define TMODE_REG           0x2A // Defines settings for the internal timer
#define TPRESCALE_REG       0x2B // Defines settings for the internal timer
#define TRELOAD_MSB_REG     0x2C // Defines the 16-bit timer reload value
#define TRELOAD_LSB_REG     0x2D // Defines the 16-bit timer reload value
#define TCOUNTVAL_MSB_REG   0x2E // Shows the 16-bit timer value
#define TCOUNTVAL_LSB_REG   0x2F // Shows the 16-bit timer value

// Test register
#define TEST_SEL1_REG       0x31 // General test signal configuration
#define TEST_SEL2_REG       0x32 // General test signal configuration and PRBS control
#define TEST_PIN_EN_REG     0x33 // Enables pin output driver on pins D1 to D7
#define TEST_PIN_VALUE_REG  0x34 // Defines the values for D1 to D7 when it is used as an I/O bus
#define TEST_BUS_REG        0x35 // Shows the status if the internal test bus
#define AUTO_TEST_REG       0x36 // Controls the digital self test
#define VERSION_REG         0x37 // Shows the software version
#define ANALOG_TEST_REG     0x38 // Controls the pins AUX1 and AUX2
#define TEST_DAC1_REG       0x39 // Defines the test value for TESTDAC1
#define TEST_DAC2_REG       0x3A // Defines the test value for TESTDAC2
#define TEST_ADC_REG        0x3B // Shows the value of the ADC I and Q channels

/*MFRC522 Command overview*/
#define IDLE                0x00 // No action, canculs current command execution
#define MEM                 0x01 // Stores 25 bytes into the internal buffer
#define GEN_RANDOM_ID       0x02 // Generates a 10-byte random ID buffer
#define CALC_CRC            0x03 // Activates the CRC coprocessor or perfomrs a self test
#define TRANSMIT            0x04 // Transmits data from the FIFO buffer
#define NO_CMD_CHANGE       0x07 // No command change, can be used to modify the CommandReg register bits without affecting the command
#define RECEIVE             0x08 // Transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define MF_AUTH             0x0E // Performs the MIFARE standard authentication as a reader
#define SOFT_RESET          0x0F // Resets the MFRC522

/*Mifare basic function library*/
#define REQ_IDL             0x26 // REQA (idle card will respond)
#define REQ_ALL             0x52 // WUPA (All cards will respond IDLE and Halt)
#define ANTICOLL1           0x93 // Cascade level 1
#define ANTICOLL2           0x95 // Cascade level 2
#define ANTICOLL3           0x97 // Cascade level 3
#define AUTHENT1A           0x60 // Authentication with KeyA
#define AUTHENT1B           0x61 // Authentication with KeyB
#define READ                0x30 // Read 16 byte datablock
#define WRITE               0xA0 // Write 16 byte datablock
#define DECREMENT           0xC0 // Decrement value block
#define INCREMENT           0xC1 // Increment value block
#define RESTORE             0xC2 // Move value block to internal data register
#define TRANSFER            0xB0 // Move internal data-reg to value block
#define HALT                0x50 // +00

#ifndef _MFRC522_H
#define _MFRC522_H
uint8_t * selfTest ();
#endif