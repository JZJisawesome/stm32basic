#include "spiio_video.h"

#include <stdint.h>
#include <stdbool.h>

#include "bluepill.h"

//Static vars
static volatile uint16_t buffer[SPIIO_VIDEO_BUFFER_SIZE];//Read by DMA
static uint16_t pointer = 0;//Software pointer (not DMA pointer)

//Functions

void SPIIO_video_init()
{
    //Set PB14 as gpio output
    GPIOB_CRH = (GPIOB_CRH & 0xF0FFFFFF) | 0x03000000;
    
    //16 bit data frame, hardware slave management, MSBFIRST, enable SPI, slave mode, CPOL = 0, CPHA = 0
    SPI2_CR1 = 0b0000100001000000;
    SPI2_CR2 = 0b00000001;//DMA request on rxne
    
    //DMA setup input setup
    DMA_CPAR4 = (uint32_t)(&SPI2_DR);
    DMA_CMAR4 = (uint32_t)(buffer);
    DMA_CNDTR4 = SPIIO_VIDEO_BUFFER_SIZE;
    //Med-high (3/4) priority, 16 bit memory and peripheral transfers, memory increment, circular mode, read from peripheral, enable channel
    DMA_CCR4 = 0b0010010110100001;
    
    GPIOB_BSRR = 1 << 14;//Set PB14 high to signal to cpu that we're ready
}

bool SPIIO_empty()//If buffer is empty
{
    //Assumes no overruns
    return pointer == (SPIIO_VIDEO_BUFFER_SIZE - DMA_CNDTR4);
}

uint16_t SPIIO_pop()//Only call this if SPIIO_empty() is false
{
    uint16_t data = buffer[pointer];
    
    if (pointer >= (SPIIO_VIDEO_BUFFER_SIZE - 1))
        pointer = 0;
    else
        ++pointer;
    
    return data;
}
