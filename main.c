/*
TODO: 

1. Figure out how to do another eqaution after, ans?

2. clear rows after doubling back to start?

3. add parentheis and exponents?
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
    I2C1_init(system_frequency);
    GPIO_init();
    I2C_LCD my_LCD = LCD_4bit_init(0x3F, system_frequency, 4, 20);
    lcd_clear(&my_LCD);
    lcd_set_cursor(&my_LCD);

    //index is value eg: [2] is value 2 but pin 10
    uint8_t num_input_pins[10] = {0,1,10,11,4,5,6,7,8,9};//GPIOA

    // pin index should match operator index
    uint8_t operator_input_pins[5] = {0,1,2,3,4};//GPIOB
    char operators[5] = {'+','-','*','/','='};

    char* equation_str = calloc(50, sizeof(char));

    while(1){
        
        for(int i = 0; i<10; i++){
            if(GPIOA_IDR & (1<<num_input_pins[i])){
                num_pressed(&my_LCD, i, equation_str);
            }
        }

        for(int i = 0; i<sizeof(operators); i++){
            if(GPIOB_IDR & (1<<operator_input_pins[i])){
                operator_pressed(&my_LCD, operators[i], equation_str);
            }
        }   
    }
    return 0;
}