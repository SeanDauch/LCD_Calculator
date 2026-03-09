/*
TODO:

1. get busyflag working for lcd so it doesnt have to rely on delay
2. work out errors eg no x+ on lcd

*/

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>
#include "main.h"

void GPIO_init(){
    // make sure gpio ports are on
    RCC_AHB1ENR |= 1|(1<<1);

    // make sure inputs
    GPIOA_MODER &= ~(3|(3<<2)|(3<<20)|(3<<22)|(3<<8)|(3<<10)|(3<<12)|(3<<14)|(3<<16)|(3<<18));
    GPIOB_MODER &= ~(3|(3<<2)|(3<<4)|(3<<6)|(3<<8));

    // set num pins to pulldown
    GPIOA_PUPDR &= ~(3|(3<<2)|(3<<20)|(3<<22)|(3<<8)|(3<<10)|(3<<12)|(3<<14)|(3<<16)|(3<<18));
    GPIOA_PUPDR |= 2|(2<<2)|(2<<20)|(2<<22)|(2<<8)|(2<<10)|(2<<12)|(2<<14)|(2<<16)|(2<<18);

    //set operator pins to pull down
    GPIOB_PUPDR &= ~(3|(3<<2)|(3<<4)|(3<<6)|(3<<8));
    GPIOB_PUPDR |= 2|(2<<2)|(2<<4)|(2<<6)|(2<<8);
}


int main(){
    I2C1_init();
    GPIO_init();
    LCD_4bit_init();
    // clear lcd with (delay very important)
    lcd_send_cmd(0b00000001);
    delay_SysTick(2);

    uint16_t num_input_pins = 1|(1<<1)|(1<<10)|(1<<11)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9);
    uint16_t op_input_pins = 1|(1<<1)|(1<<2)|(1<<3)|(1<<4);

    char* equation_str = calloc(50, sizeof(char));

    while(1){
        
        //switch statement that checks for num presses
        switch(GPIOA_IDR & num_input_pins){
            case (1<<0):
                num_pressed(0, equation_str);
                break;
            case (1<<1):
                num_pressed(1, equation_str);
                break;
            case (1<<10):
                num_pressed(2, equation_str);
                break;
            case (1<<11):
                num_pressed(3, equation_str);
                break;
            case (1<<4):
                num_pressed(4, equation_str);
                break;
            case (1<<5):
                num_pressed(5, equation_str);
                break;
            case (1<<6):
                num_pressed(6, equation_str);
                break;
            case (1<<7):
                num_pressed(7, equation_str);
                break;
            case (1<<8):
                num_pressed(8, equation_str);
                break;
            case (1<<9):
                num_pressed(9, equation_str);
                break;
            default:
                break;
        }
        
        //switch statement for operators
        switch (GPIOB_IDR & op_input_pins){
            case (1<<0):
                operator_pressed('+', equation_str);
                break;
            case (1<<1):
                operator_pressed('-', equation_str);
                break;
            case (1<<2):
                operator_pressed('*', equation_str);
                break;
            case (1<<3):
                operator_pressed('/', equation_str);
                break;
            case (1<<4):
                operator_pressed('=', equation_str);

                double answer = str_to_ans(equation_str);
                lcd_print_double(answer);
                break;
            default:
                break;
        }
        
    }
    return 0;
}