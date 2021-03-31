#include "ps2uart_cpu.h"

#include "bluepill.h"

void PS2UART_cpu_init()
{
    //Set PA10 as input
    //GPIOA_CRH = (GPIOA_CRH & 0xFFFFF0FF) | 0x00000400;//Already input by default
}

bool PS2UART_empty()//If buffer is empty
{
    return true;//TEST
}

uint8_t PS2UART_pop()//Only call this if PS2_empty() is false
{
    return 0;//TEST
}
