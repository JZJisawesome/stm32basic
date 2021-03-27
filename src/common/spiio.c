#include "spiio.h"

#include "bluepill.h"

//Static variables

static volatile uint16_t inBuffer[SPIIO_BUFFER_SIZE];//Fed by DMA
static volatile uint16_t outBuffer[SPIIO_BUFFER_SIZE];//Read by DMA
static volatile uint16_t inPointer = 0;//Software pointer (not DMA pointer)
static volatile uint16_t outPointer = 0;//Software pointer (not DMA pointer)

static volatile bool recievedEXTIInterrupt = false;

//Static functions

/*
static void SPIIO_extiInit()
{
    //Set PA1 as push-pull gpio output and leave PA0 as input
    GPIOA_CRL = (GPIOA_CRL & 0xFFFFFF0F) | 0x00000030;
    //TODO setup external interrupts
}*/

//Common functions

//Functions for video

void SPIIO_video_init()
{
    //SPIIO_extiInit();
    
    //Set PB14 (MISO) as AF output
    GPIOB_CRH = (GPIOB_CRH & 0xF0FFFFFF) | 0x0B000000;
    
    //16 bit data frame, hardware slave management, MSBFIRST, enable SPI, slave mode, CPOL = 0, CPHA = 0
    SPI2_CR1 = 0b0000100001000000;
    SPI2_CR2 = 0b00000001;
    
    //DMA setup
    DMA_CPAR4 = (uint32_t)(&SPI2_DR);
    DMA_CMAR4 = (uint32_t)(inBuffer);
    DMA_CNDTR4 = SPIIO_BUFFER_SIZE;
    //Med priority, 16 bit memory and peripheral transfers, memory increment, circular mode, read from peripheral, enable channel
    DMA_CCR4 = 0b0001010110100001;
    
    //TODO setup DMA channel 5 for sending data to cpu mcu
    
    //TODO signal to cpu with positive edge that video is ready
}

bool SPIIO_video_empty()//If in buffer is empty
{
    //Assumes no overruns
    return inPointer == (SPIIO_BUFFER_SIZE - DMA_CNDTR4);
}

uint16_t SPIIO_video_pop()
{
    uint16_t data = inBuffer[inPointer];
    
    if (inPointer >= (SPIIO_BUFFER_SIZE - 1))
        inPointer = 0;
    else
        ++inPointer;
    
    return data;
}

//Functions for cpu

void SPIIO_cpu_init()
{
}

/*
void SPIIO_cpu_spiInit()
{
    SPIIO_extiInit();
    
    //TODO setup SPI in the mean time, then wait for an interrupt from the video mcu
    
    while (!recievedEXTIInterrupt);
    recievedEXTIInterrupt = false;
}*/

//Interrupts
/*
__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI0()
{
    recievedEXTIInterrupt = true;//Got an interrupt
}*/
