#include <stdint.h>
#include "../main.h"

// simple delay function in ms
void delay_SysTick(uint32_t delay_ms){
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