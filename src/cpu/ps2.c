#include "ps2.h"

#include "bluepill.h"

static volatile uint8_t buffer[PS2_BUFFER_SIZE];
static uint16_t popPointer = 0;//Next place to pop from
static volatile uint16_t pushPointer = 0;//Next place to push to

void PS2_init()
{
    //Set PB4 and PB7 high, then as open-collector outputs
    GPIOB_BSRR = 0b0000000010010000;
    GPIOB_CRL |= 0x70070000;
    
    //Initialize EXTI3 for PB4 for negative edges
    AFIO_EXTICR2 |= 0x00000001;//Map PB4 as exti 4
    EXTI_FTSR |= (1 << 4);//Set exti 4 to negative edge
    EXTI_IMR |= (1 << 4);//Disable interrupt mask for exti 4
    NVIC_ISER0 = (1 << 10);//Enable exti 4 in the nvic
}

bool PS2_empty()//If buffer is empty
{
    return popPointer == pushPointer;
}

uint8_t PS2_pop()//Only call this if PS2_empty() is false
{
    
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI4()//Fires every negative edge of the clock
{
    static enum {START = -1, PARITY = 8, STOP = 9} state = START;//States 0 to 7 are bits 0 to 7
    
    uint32_t portBPins = GPIOB_IDR;//Capture PB7
    
    EXTI_PR = 1 << 4;//Clear the pending register
    
    //TESTING
    GPIOC_BSRR = 1 << 13;
    __delayInstructions(40);
    GPIOC_BRR = 1 << 13;
    
    
    
    
    switch (state)
    {
        case START:
        {
            buffer[pushPointer] = 0;//So that we can or in bits
            break;
        }
        case PARITY:
        {
            //TODO check parity
            break;
        }
        case STOP:
        {
            if (pushPointer == (PS2_BUFFER_SIZE - 1))
                pushPointer = 0;
            else
                ++pushPointer;
            
            break;
        }
        default://Storing a bit
        {
            bool bit = (portBPins >> 7) & 1;//Move PB7 to first bit position
            buffer[pushPointer] |= bit << state;//Or-in bit into proper position
            
            break;
        }
    }
    
    if (state == STOP)
        state = START;
    else
        ++state;
    
    
}
