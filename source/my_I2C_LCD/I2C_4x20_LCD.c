#include "I2C_4x20_LCD.h"
#include <stdio.h> // need for snprintf

// busyflag doesnt work with I2C backpack

#define RCC_Base 0x40023800
#define RCC_AHB1ENR *((volatile uint32_t*)(RCC_Base + 0x30))
// I2C lives in APB1
#define RCC_APB1ENR *((volatile uint32_t*)(RCC_Base + 0x40))
// reset APB1
#define RCC_APB1RSTR *((volatile uint32_t*)(RCC_Base + 0x20))

#define GPIOB_Base 0x40020400
#define GPIOB_MODER *((volatile uint32_t*)(GPIOB_Base + 0x00))
#define GPIOB_OTYPER *((volatile uint32_t*)(GPIOB_Base + 0x04))
#define GPIOB_OSPEEDR *((volatile uint32_t*)(GPIOB_Base + 0x08))
#define GPIOB_PUPDR *((volatile uint32_t*)(GPIOB_Base + 0x0C))
// says what kind of alternate function (H means pins 8-15)
#define GPIOB_AFRH *((volatile uint32_t*)(GPIOB_Base + 0x24))

#define I2C1_Base 0x40005400
#define I2C1_CR1 *((volatile uint32_t*)(I2C1_Base + 0x00))
#define I2C1_CR2 *((volatile uint32_t*)(I2C1_Base + 0x04))
#define I2C1_CCR *((volatile uint32_t*)(I2C1_Base + 0x1C))
#define I2C1_TRISE *((volatile uint32_t*)(I2C1_Base + 0x20))
#define I2C1_SR1 *((volatile uint32_t*)(I2C1_Base + 0x14))
#define I2C1_SR2 *((volatile uint32_t*)(I2C1_Base + 0x18))
#define I2C1_DR *((volatile uint32_t*)(I2C1_Base + 0x10))

// simple delay function in ms
static void delay_SysTick(uint32_t delay_ms, uint32_t sys_freq){
    // SysTick Reload Value Register (specifies value to count down from)
    volatile uint32_t* pSYST_RVR = (volatile uint32_t*)(0xE000E014);
    // beats every 1 ms
    *pSYST_RVR = (sys_freq/1000) - 1;

    volatile uint32_t* pSYST_CSR = (volatile uint32_t*)(0xE000E010);
    // tells that we're using proccessor clock
    *pSYST_CSR |= (1<<2);
    // TICKINT, I dont really understand, if bit 1 then it goes to cpu 
    // interupt handler, otherwise just deal with it as software
    *pSYST_CSR &= ~(1<<1);
    // Enables the counter
    *pSYST_CSR |= (1<<0);

    for (int i = 0; i < delay_ms; i++){
        // COUNTFLAG Returns 1 if timer counted to 0 since last time this was read.
        while((*pSYST_CSR & (1<<16)) == 0){}
    } 

    // Disables the counter
    *pSYST_CSR &= ~(1<<0);
}

// Initializes pins B_8 and B_9 for I2c1
void I2C1_init(uint32_t APB1_freq){
    uint32_t freq_mhz = APB1_freq/1000000;

    // enable clocks
    RCC_AHB1ENR |= (1<<1);// enable rcc for port B
    RCC_APB1ENR |= (1<<21);// I2C1 clock enable

    // clears I2C
    RCC_APB1RSTR |= (1<<21);
    RCC_APB1RSTR &= ~(1<<21);
    

    // initailize PB8/PB9 for I2C1
    GPIOB_MODER &= ~((3<<16) | (3<<18)); // make sure theyre clear
    GPIOB_MODER |= (2<<16) | (2<<18);

    GPIOB_AFRH &= ~((15<<0)|(15<<4)); // clear
    GPIOB_AFRH |= (1<<2) | (1<<6);

    // set both pins to be open drain
    GPIOB_OTYPER |= (1<<8) | (1<<9);

    // set to fastest speed
    GPIOB_OSPEEDR |= (3<<16) | (3<<18);
    
    // redundant pull up resistors (still use external)
    GPIOB_PUPDR &= ~((3<<16) | (3<<18));
    GPIOB_PUPDR |= (1<<16) | (1<<18);
    
    // initialize I2C clock
    I2C1_CR1 &= ~(1<<0); // turn off I2C1 so it doesnt geek while configuring

        // tells I2C how fast system is moving
        I2C1_CR2 &= ~(0x3F); // clear first 6 bits
        I2C1_CR2 |= freq_mhz; 

        // tells how fast to move scl signals
        I2C1_CCR &= ~((3<<14)|(0xFFF)); // clear
        I2C1_CCR &= ~(1<<15); // standard mode (scl_freq == 100kHz)
        I2C1_CCR |= APB1_freq/(2*100000); // CCR = PCLK/(2*SCL_Freq)  

        I2C1_TRISE &= ~(0x3F);
        I2C1_TRISE |= freq_mhz + 1; // max amount of time that scl is allowed to rise

    I2C1_CR1 |= (1<<0); // turn back on
}

static void I2C1_send(uint8_t address, uint8_t data_byte){
    // 1-1. Set start bit
    I2C1_CR1 |= (1<<8);
    // 1-2. Wait for start bit
    while(!(I2C1_SR1 & (1<<0))){}

    // 2-1. Send address
    I2C1_DR = (address<<1); // has to be shifted over for r/w bit (w=0, r=1)
    // 2-2. Wait for address to send
    while(!(I2C1_SR1 & (1<<1))){}

    // read I2C_SR1/2 to clear ADDR bit
    int temp = I2C1_SR1;
    temp = I2C1_SR2;
    (void)temp;//suppresses warning


    // 3-1. Wait empty data register
    while(!(I2C1_SR1 & (1<<7))){}
    // 3-2. Write byte
    I2C1_DR = data_byte;
    // 3-3. Wait for byte transfer finish
    while(!(I2C1_SR1 & (1<<2))){}

    // 4. Stop condition
    I2C1_CR1 |= (1<<9);
}

/* The busyflag just doesnt work with the I2c backpack
// receives one byte
static uint8_t I2C1_receive(uint8_t address){
    // 1-1. Set start bit
    I2C1_CR1 |= (1<<8);
    // 1-2. Wait for start bit
    while(!(I2C1_SR1 & (1<<0))){}

    // 2-1. Send address
    I2C1_DR = (address<<1)|1; // has to be shifted over for r/w bit (w=0, r=1)
    // 2-2. Wait for address to send
    while(!(I2C1_SR1 & (1<<1))){}

    // clear ack (needed to generate nack)
    I2C1_CR1 &= ~(1<<10);

    // read I2C_SR1/2 to clear ADDR bit
    int temp = I2C1_SR1;
    temp = I2C1_SR2;
    (void)temp;//suppresses warning

    // 4. Stop condition
    I2C1_CR1 |= (1<<9);

    // 3-1. Wait empty data register
    while(!(I2C1_SR1 & (1<<6))){}
    // 3-2. Read data register byte
    return I2C1_DR;
}

static uint8_t read_busyflag(uint8_t address){

    // gets the 4 high bits
    // set enable high
    I2C1_send(address,0b11111110);// 0b(D/D/D/D/light/Enable/rw/rs)
    // read busyflag
    uint8_t busyflag_high = I2C1_receive(address);
    // set enable low
    I2C1_send(address,0b11111010);

    // gets the 4 low bits (need to get all bits to avoid error)
    // set enable high
    I2C1_send(address,0b11111110);
    // read busyflag
    uint8_t busyflag_low = I2C1_receive(address);
    // set enable low
    I2C1_send(address,0b11111010);



    if(busyflag_high & (1<<7)){
        // busy condition
        return 1;
    }else{
        return 0;
    }
}*/

// send the 4 bit version, with the enable(E) acting as a shutter
static void nybble(uint8_t address, uint8_t bin_num, uint8_t RS_bit){
    uint8_t backlight = (1<<3);
    uint8_t enable = (1<<2);
    uint8_t write = (0<<1);

    uint8_t data_en_high = (bin_num & 0b11110000)|backlight|enable|write|(RS_bit<<0);
    I2C1_send(address, data_en_high);

    uint8_t data_en_low = data_en_high & ~enable;
    I2C1_send(address, data_en_low);
}

// breaks the 8 bits into 4 bit packages
static void lcd_send_cmd(uint8_t address, uint8_t cmd){
    uint8_t high = (cmd & 0xF0);//0xF0 = 11110000
    uint8_t low = (cmd << 4);

    nybble(address, high, 0);
    nybble(address, low, 0);
}

// breaks the 8 bits into 4 bit packages
static void lcd_send_data(uint8_t address, uint8_t data){
    uint8_t high = (data & 0xF0);//0xF0 = 11110000
    uint8_t low = (data << 4);

    nybble(address, high, 1);
    nybble(address, low, 1);
}

// based off psudeocode found in random datasheet
I2C_LCD LCD_4bit_init(uint8_t address, uint64_t sys_freq){
    delay_SysTick(50, sys_freq);
    nybble(address, 0x30, 0);
    delay_SysTick(10, sys_freq);
    nybble(address, 0x30, 0);
    delay_SysTick(10, sys_freq);
    nybble(address, 0x30, 0); 
    delay_SysTick(10, sys_freq);
        
    // finish wake up commands

    nybble(address, 0x20, 0);// enter 4 bit mode
    delay_SysTick(10, sys_freq);
    lcd_send_cmd(address, 0x28);
    lcd_send_cmd(address, 0x10);
    lcd_send_cmd(address, 0x0F);
    lcd_send_cmd(address, 0x06);

    I2C_LCD lcd = {address, sys_freq};
    return lcd;
}

void lcd_print_char(I2C_LCD* lcd, char letter){
    lcd_send_data(lcd-> address, (int)letter);
    delay_SysTick(1, lcd->sys_freq);
}

void lcd_print_string(I2C_LCD* lcd, char* str){
    for(int i = 0; str[i] != '\0'; i++){
        lcd_print_char(lcd, str[i]);
    }
}

void lcd_print_double(I2C_LCD* lcd, double num_in){
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.3g", num_in);
    lcd_print_string(lcd, buffer);
}

void lcd_clear(I2C_LCD* lcd){
    lcd_send_cmd(lcd->address, 0b00000001);
    delay_SysTick(2, lcd->sys_freq);
}

void lcd_cursor_home(I2C_LCD* lcd){
    lcd_send_cmd(lcd->address, 0b00000011);
    delay_SysTick(2, lcd->sys_freq);
}