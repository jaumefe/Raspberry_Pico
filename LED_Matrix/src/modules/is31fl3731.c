#include "is31fl3731.h"

uint8_t buf[145];
const uint8_t GAMMA_8BIT[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
    6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
    11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
    19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
    29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
    40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
    71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
    90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
    111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
    135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
    162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
    191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
    222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255
};

bool IS31FL3731_init() {
    uint8_t cmd[2] = {BANK, CONFIG_BANK};
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);
    sleep_ms(10);
    cmd[0] = SHUTDOWN;
    cmd[1] = 0b00000000;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);
    
    IS31FL3731_clear();
    IS31FL3731_update(0);

    cmd[1] = 0b00000001;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);

    cmd[0] = MODE;
    cmd[1] = PICTURE;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);

    cmd[0] = AUDIOSYNC;
    cmd[1] = 0;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);

    return true;
}

void IS31FL3731_clear() {
    for (auto i = 0u; i < sizeof(buf); i++){
        buf[i] = 0;
    }
}

void IS31FL3731_update(uint8_t frame) {
    uint8_t cmd[2] = {BANK, frame};
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);

    buf[0] = COLOR_OFFSET;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, buf, sizeof(buf), false);

    cmd[1] = CONFIG_BANK;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);

    cmd[0] = FRAME;
    cmd[1] = frame;
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);
}

void IS31FL3731_enable(uint8_t * list, uint8_t frame){
    uint8_t cmd[2] = {BANK, frame};
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, cmd, 2, false);
    
    uint8_t new_cmd[] = {};
    new_cmd[0] = ENABLE_OFFSET;
    for (uint8_t i = 1; i <= sizeof(list); i++){
        new_cmd[i] = list[i-1];
    }
    i2c_write_blocking(i2c_default, DEFAULT_I2C_ADDRES, new_cmd, sizeof(new_cmd), false);
}

void IS31FL3731_set(uint8_t index, uint8_t brightness){
    buf[index + 1] = GAMMA_8BIT[brightness];
}