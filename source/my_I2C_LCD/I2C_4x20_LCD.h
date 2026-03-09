#ifndef _I2C_4x20_LCD_h
    #define _I2C_4x20_LCD_h

    #include <stdint.h>

    void I2C1_init();
    void lcd_send_cmd(uint8_t cmd);
    void lcd_send_data(uint8_t data);
    void LCD_4bit_init();
    void lcd_print_string(char* str);
    void lcd_print_double(double num_in);

#endif