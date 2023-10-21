#include <stdio.h>
#include "pico_liquidcrystal.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

void liquidCrystalI2C_init (struct LiquidCrystal_I2C * lcd){
    lcd->displayFunction = LCD_4BIT_MODE | LCD_1LINE | LCD_5x8DOTS;
    begin(lcd);
}

void begin (struct LiquidCrystal_I2C * lcd){
    uint8_t lines = lcd->rows;
    if (lines > 1) {
        lcd->displayFunction |= LCD_2LINE;
    }
    lcd->numLines = lines;

    // For some 1 line displays it can be selected a 10 pixel high font
    if ((lcd->charSize != 0) && (lcd->numLines == 1)){
        lcd->displayFunction |= LCD_5x10DOTS;
    }

    sleep_ms(50);

    // Now we pull both RS and R/W low to begin commands
    expanderWrite(lcd->backlightVal, lcd);
    sleep_ms(1000);

    // Put the LCD into 4 bit mode

    write4bits(0x03 << 4, lcd);
    sleep_us(4500);

    // Second try
    write4bits(0x03 << 4, lcd);
    sleep_us(4500);

    // Third try
    write4bits(0x03 << 4, lcd);
    sleep_us(4500);

    // Finally, set to 4-bit interface
    write4bits(0x02 << 4, lcd);

    // Set # lines, font size, etc.
    command(LCD_FUNC_SET | lcd->displayFunction, lcd);

    // Turn on the display on with no cursor or blinking default
    lcd->displayControl = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
    display(lcd);

    // Clear it off
    clear(lcd);

    // Initialize to default text direction (for roman language)
    lcd->displayMode = LCD_ENTRYLEFT | LCD_ENTRY_SHT_DECR;

    // Set the entry mode
    command(LCD_MODE_SET, lcd);

    home(lcd);
}

/********** high level commands, for the user! */
void clear(struct LiquidCrystal_I2C * lcd){
    command(LCD_CLR_DISP, lcd); // clear display, set cursor position to zero
    sleep_us(2000);
}

void home(struct LiquidCrystal_I2C * lcd){
    command(LCD_RET_HOME, lcd); // set cursor position to zero
    sleep_us(2000);
}

void setCursor(uint8_t col, uint8_t row, struct LiquidCrystal_I2C * lcd){
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > lcd->numLines) {
        row = lcd->numLines - 1;
    }
    command(LCD_SETDDRANADDR | (col + row_offsets[row]), lcd);
}

// Turn the display on/off (quickly)
void noDisplay(struct LiquidCrystal_I2C * lcd){
    lcd->displayControl &= ~LCD_DISPLAY_ON;
    command(LCD_DISP_CTRL | lcd->displayControl, lcd);
}

void display (struct LiquidCrystal_I2C * lcd){
    lcd->displayControl |= LCD_DISPLAY_ON;
    command(LCD_DISP_CTRL | lcd->displayControl, lcd);
}

// Turns the underline cursor on/off
void noCursor(struct LiquidCrystal_I2C * lcd){
    lcd->displayControl &= ~LCD_CURSOR_ON;
    command(LCD_DISP_CTRL | lcd->displayControl, lcd);
}

void cursor(struct LiquidCrystal_I2C * lcd){
    lcd->displayControl |= LCD_CURSOR_ON;
    command(LCD_DISP_CTRL | lcd->displayControl, lcd);
}

// Turn on and off the blinking cursor
void noBlink(struct LiquidCrystal_I2C * lcd){
    lcd->displayControl &= ~LCD_BLINK_ON;
    command(LCD_DISP_CTRL | lcd->displayControl, lcd);
}

void blink(struct LiquidCrystal_I2C * lcd){
    lcd->displayControl |= LCD_BLINK_ON;
    command(LCD_DISP_CTRL | lcd->displayControl, lcd);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(struct LiquidCrystal_I2C * lcd){
    command(LCD_CUR_SHFT | LCD_DISPLAY_MV | LCD_MV_LEFT, lcd);
}

void scrollDisplayRight(struct LiquidCrystal_I2C * lcd){
    command(LCD_CUR_SHFT | LCD_DISPLAY_MV | LCD_MV_RIGHT, lcd);
}

// This is for text that flows Left to Right
void leftToRight(struct LiquidCrystal_I2C * lcd){
    lcd->displayMode |= LCD_ENTRYLEFT;
    command(LCD_MODE_SET | lcd->displayMode, lcd);
}

// This is for text that flows Right to Left
void rightToLeft(struct LiquidCrystal_I2C * lcd){
    lcd->displayMode &= ~LCD_ENTRYLEFT;
    command(LCD_MODE_SET | lcd->displayMode, lcd);
}

// This will 'right justify' text from the cursor
void autoscroll(struct LiquidCrystal_I2C * lcd){
    lcd->displayMode |= LCD_ENTRY_SHT_INCR;
    command(LCD_MODE_SET | lcd->displayMode, lcd);
}

// This will 'left justify' text from the cursor
void noAutoscroll(struct LiquidCrystal_I2C * lcd){
    lcd->displayMode &= ~LCD_ENTRY_SHT_INCR;
    command(LCD_MODE_SET | lcd->displayMode, lcd);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
/*void createChar(uint8_t location, uint8_t charmap[]){
    location &= 0x7; // We only have 8 locations: 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for(int i = 0; i < 8; i++){
        write(charmap[i]);
    }
}

// createChar with PROGMEM input
void createChar(uint8_t location, const char * charmap){
    location &= 0x7; // We only have 8 locations: 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for(int i = 0; i < 8; i++){
        write(pgm_read_byte_near(charmap++));
    }
}*/

// Turn the (optional) backlight off/on
void noBacklight(struct LiquidCrystal_I2C * lcd){
    lcd->backlightVal=LCD_NO_BACKLIGHT;
    expanderWrite(0, lcd);
}

void backlight(struct LiquidCrystal_I2C * lcd){
    lcd->backlightVal=LCD_BACKLIGHT;
    expanderWrite(0, lcd);
}

/*********** mid level commands, for sending data/cmds */
void command(uint8_t value, struct LiquidCrystal_I2C * lcd){
    send(value, 0, lcd);
}

/************ low level data pushing commands **********/
void send(uint8_t value, uint8_t mode, struct LiquidCrystal_I2C * lcd){
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    write4bits((highnib) | mode, lcd);
    write4bits((lownib) | mode, lcd);
}

void write4bits(uint8_t value, struct LiquidCrystal_I2C * lcd){
    expanderWrite(value, lcd);
    pulseEnable(value, lcd);
}

void expanderWrite(uint8_t data, struct LiquidCrystal_I2C * lcd){
    uint8_t reg = data | lcd->backlightVal;
    i2c_write_blocking(i2c_default, lcd->addr, &reg, 1, false);
}

void pulseEnable (uint8_t data, struct LiquidCrystal_I2C * lcd){
    expanderWrite(data | EN, lcd);   // EN high
    sleep_us(1);                // Enable pulse must be > 450ms

    expanderWrite(data & ~EN, lcd);  // EN low
    sleep_us(50);               //Comands need > 37us to settle
}