#include "ps2uart_cpu.h"

#include "bluepill.h"

static volatile uint8_t buffer[PS2_BUFFER_SIZE];
static uint16_t pointer = 0;//Software pointer (not DMA)

void PS2UART_cpu_init()
{
    //TODO maybe add another line that signals the cpu is ready for uart transfers?
    
    //Set PA3 as input
    //GPIOA_CRL = (GPIOA_CRH & 0xFFFF0FFF) | 0x00004000;//Already input by default
    
    //Setup DMA
    DMA_CPAR6 = (uint32_t)(&USART2_DR);
    DMA_CMAR6 = (uint32_t)(buffer);
    DMA_CNDTR6 = PS2_BUFFER_SIZE;
    //Max (4/4) priority, 8 bit memory and peripheral transfers, memory increment, circular mode, read from peripheral, enable channel
    DMA_CCR6 = 0b0011000010100001;
    
    //Setup uart
    USART2_BRR = 0x0010;//Set baud rate to 2.25mbit
    USART2_CR3 = 0b0000000001000000;//Enable receiving using DMA
    USART2_CR1 = 0b0010000000000100;//Enable uart (receiver only, 8 data bits)
}

bool PS2UART_empty()//If buffer is empty
{
    return pointer == (PS2_BUFFER_SIZE - DMA_CNDTR6);
}

uint8_t PS2UART_pop()//Only call this if PS2_empty() is false
{
    uint8_t data = buffer[pointer];
    
    if (pointer == (PS2_BUFFER_SIZE - 1))
        pointer = 0;
    else
        ++pointer;
    
    return data;
}
