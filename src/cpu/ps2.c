#include "ps2.h"

#include "bluepill.h"

uint8_t buffer[PS2_BUFFER_SIZE];
uint16_t popPointer = 0;
uint16_t pushPointer = 0;

void PS2_init()
{
    //Set PB3 and PB4 high, then as open-collector outputs
    GPIOB_BSRR = 0b0000000000011000;
    GPIOB_CRL |= 0x00033000;//Both PB3 and PB4 become 4 | 3 = 7 = open-collector
    
    //Initialize EXTI3 for PB3 for negative edges
    //FIXME something is broken here
    AFIO_EXTICR1 |= 0x1000;//Map PB3 as exti 3
    EXTI_FTSR |= (1 << 3);//Set exti 3 to negative edge
    EXTI_IMR |= (1 << 3);//Disable interrupt mask for exti 3
    NVIC_ISER0 = (1 << 9);//Enable exti 3 in the nvic
}

bool PS2_empty()//If buffer is empty
{
    
}

uint8_t PS2_pop()//Only call this if PS2_empty() is false
{
    
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI3()//Fires every negative edge of the clock
{
    EXTI_PR = 1 << 3;//Clear the pending register
    
    GPIOC_BSRR = 1 << 13;
    __delayInstructions(40000000);
    GPIOC_BRR = 1 << 13;
    
    uint32_t portBPins = GPIOB_IDR;
    bool data = (portBPins >> 4) & 1;//PB4
    //while (true);//TESTING
}
