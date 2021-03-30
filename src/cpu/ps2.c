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
    uint8_t data = buffer[popPointer];
    
    if (popPointer == (PS2_BUFFER_SIZE - 1))
        popPointer = 0;
    else
        ++popPointer;
    
    return data;
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI4()//Fires every negative edge of the clock
{
    uint32_t portBPins = GPIOB_IDR;//Capture PB7
    EXTI_PR = 1 << 4;//Clear the pending register
    
    //NOTE: For some reason most of my dev boards like to trigger on the posedge and negedge
    //even though EXTI_RTSR is not set. You can test if this is a problem for you by
    //commenting out this if statement with !(portBPins & (1 << 4))
    if (!(portBPins & (1 << 4)))
    {
        static enum {START = -1, PARITY = 8, STOP = 9} state = START;//States 0 to 7 are bits 0 to 7
        
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
}

char PS2_toAscii(uint16_t keyboardData, bool capital)
{
    #if KEYBOARD_SET == 1
        if (capital)
        {
            switch (keyboardData)
            {
                //TODO
            }
        }
        else
        {
            switch (keyboardData)
            {
                //TODO
            }
        }
    #else//Keyboard set 2
        //https://techdocs.altium.com/display/FPGA/PS2+Keyboard+Scan+Codes
        if (capital)
        {
            switch (keyboardData)
            {
                //TODO
            }
        }
        else
        {
            switch (keyboardData)
            {
                
            }
        }
    #endif
    //Will return garbage if the keyboardData does not translate to ascii
}
