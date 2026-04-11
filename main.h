#ifndef _main_h
    #define _main_h

    #include <stdint.h>
    #include "source/my_I2C_LCD/I2C_4x20_LCD.h"

    #define system_frequency 16000000
    #define arr_length 25

    #define RCC_Base 0x40023800
    #define RCC_AHB1ENR *((volatile uint32_t*)(RCC_Base + 0x30))
    // I2C lives in APB1
    #define RCC_APB1ENR *((volatile uint32_t*)(RCC_Base + 0x40))
    // reset APB1
    #define RCC_APB1RSTR *((volatile uint32_t*)(RCC_Base + 0x20))

    #define GPIOA_Base 0x40020000
    #define GPIOA_PUPDR *((volatile uint32_t*)(GPIOA_Base + 0x0C))
    #define GPIOA_IDR *((volatile uint32_t*)(GPIOA_Base +0x10))
    #define GPIOA_MODER *((volatile uint32_t*)(GPIOA_Base + 0x00))

    #define GPIOB_Base 0x40020400
    #define GPIOB_MODER *((volatile uint32_t*)(GPIOB_Base + 0x00))
    #define GPIOB_OTYPER *((volatile uint32_t*)(GPIOB_Base + 0x04))
    #define GPIOB_OSPEEDR *((volatile uint32_t*)(GPIOB_Base + 0x08))
    #define GPIOB_PUPDR *((volatile uint32_t*)(GPIOB_Base + 0x0C))
    // says what kind of alternate function (H means pins 8-15)
    #define GPIOB_AFRH *((volatile uint32_t*)(GPIOB_Base + 0x24))
    #define GPIOB_IDR *((volatile uint32_t*)(GPIOB_Base +0x10))

    #define I2C1_Base 0x40005400
    #define I2C1_CR1 *((volatile uint32_t*)(I2C1_Base + 0x00))
    #define I2C1_CCR *((volatile uint32_t*)(I2C1_Base + 0x1C))
    #define I2C1_SR1 *((volatile uint32_t*)(I2C1_Base + 0x14))
    #define I2C1_SR2 *((volatile uint32_t*)(I2C1_Base + 0x18))
    #define I2C1_DR *((volatile uint32_t*)(I2C1_Base + 0x10))

    //defined in delay.c
    void delay_SysTick(uint32_t delay_ms, uint32_t sys_freq);
    
    // calc functions
    void input_to_str(char input, char* eq_str);
    void num_pressed(I2C_LCD* lcd, uint8_t input_num, char* eq_str);
    void operator_pressed(I2C_LCD* lcd, char op, char* eq_str);
#endif