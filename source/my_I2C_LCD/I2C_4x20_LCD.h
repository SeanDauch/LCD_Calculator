#ifndef _I2C_4x20_LCD_h
    #define _I2C_4x20_LCD_h

    #include <stdint.h>

    struct I2C_LCD{
    uint8_t address;
    uint32_t sys_freq;
    }typedef I2C_LCD;

    void I2C1_init(uint32_t APB1_freq);
    I2C_LCD LCD_4bit_init(uint8_t address, uint64_t sys_freq);
    void lcd_print_char(I2C_LCD* lcd, char letter);
    void lcd_print_string(I2C_LCD* lcd, char* str);
    void lcd_print_double(I2C_LCD* lcd, double num_in);
    void lcd_clear(I2C_LCD* lcd);
    void lcd_cursor_home(I2C_LCD* lcd);

#endif