#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"


#ifndef pico_liquidcrystal_h
#define pico_liquidcrystal_h

// Commands
#define LCD_CLR_DISP        0x01
#define LCD_RET_HOME        0x02
#define LCD_MODE_SET        0x04
#define LCD_DISP_CTRL       0x08
#define LCD_CUR_SHFT        0x10
#define LCD_FUNC_SET        0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRANADDR    0x80

// Flags for display entry mode
#define LCD_ENTRYRIGHT      0x00
#define LCD_ENTRYLEFT       0x02
#define LCD_ENTRY_SHT_INCR  0x01
#define LCD_ENTRY_SHT_DECR  0x00

// Flags for display on/off control
#define LCD_DISPLAY_ON      0x04
#define LCD_DISPLAY_OFF     0x00
#define LCD_CURSOR_ON       0x02
#define LCD_CURSOR_OFF      0x00
#define LCD_BLINK_ON        0x01
#define LCD_BLINK_OFF       0x00

// Flags for display/cursor shift
#define LCD_DISPLAY_MV      0x10
#define LCD_CURSOR_MV       0x00
#define LCD_MV_RIGHT        0x04
#define LCD_MV_LEFT         0x00

// Flags for function set
#define LCD_8BIT_MODE       0x10
#define LCD_4BIT_MODE       0x00
#define LCD_2LINE           0x08
#define LCD_1LINE           0x00
#define LCD_5x10DOTS        0x04
#define LCD_5x8DOTS         0x00

// Flags for backlight control
#define LCD_BACKLIGHT       0x08
#define LCD_NO_BACKLIGHT    0x00


#define EN                  0b00000100
#define RW                  0b00000010
#define RS                  0b00000001

struct LiquidCrystal_I2C{
    uint8_t addr, displayFunction, displayControl, displayMode, numLines, cols, rows, backlightVal, charSize;
};

void liquidCrystalI2C_init (struct LiquidCrystal_I2C * lcd);
void begin (struct LiquidCrystal_I2C * lcd);
void clear(struct LiquidCrystal_I2C * lcd);
void home(struct LiquidCrystal_I2C * lcd);
void setCursor(uint8_t col, uint8_t row, struct LiquidCrystal_I2C * lcd);
void noDisplay(struct LiquidCrystal_I2C * lcd);
void display (struct LiquidCrystal_I2C * lcd);
void noCursor(struct LiquidCrystal_I2C * lcd);
void cursor(struct LiquidCrystal_I2C * lcd);
void noBlink(struct LiquidCrystal_I2C * lcd);
void blink(struct LiquidCrystal_I2C * lcd);
void scrollDisplayLeft(struct LiquidCrystal_I2C * lcd);
void scrollDisplayRight(struct LiquidCrystal_I2C * lcd);
void leftToRight(struct LiquidCrystal_I2C * lcd);
void rightToLeft(struct LiquidCrystal_I2C * lcd);
void autoscroll(struct LiquidCrystal_I2C * lcd);
void noAutoscroll(struct LiquidCrystal_I2C * lcd);
void noBacklight(struct LiquidCrystal_I2C * lcd);
void backlight(struct LiquidCrystal_I2C * lcd);
void command(uint8_t value, struct LiquidCrystal_I2C * lcd);
void send(uint8_t value, uint8_t mode, struct LiquidCrystal_I2C * lcd);
void write4bits(uint8_t value, struct LiquidCrystal_I2C * lcd);
void expanderWrite(uint8_t data, struct LiquidCrystal_I2C * lcd);
void pulseEnable (uint8_t data, struct LiquidCrystal_I2C * lcd);

#endif