#include "spiio.h"

#include "bluepill.h"

#define HALF_BUFFER_SIZE (SPIIO_BUFFER_SIZE / 2)

//Static variables

static volatile uint16_t inBuffer[SPIIO_BUFFER_SIZE];//Fed by DMA
static volatile uint16_t outBuffer[SPIIO_BUFFER_SIZE];//Read by DMA
static uint16_t inPointer = 0;//Software pointer (not DMA pointer)
static uint16_t outPointer = 0;//Software pointer (not DMA pointer)
//static volatile uint16_t dmaOutPointer = 0;//DMA pointer
//static volatile uint16_t dmaCount;

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
    SPI2_CR2 = 0b00000001;//DMA request on rxne
    
    //DMA setup input setup
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
    //Set PB15, PB13 (MOSI, SCK) as AF outputs; PB12 as gpio output
    //TODO do in one operation to avoid glitches
    GPIOB_CRH = (GPIOB_CRH & 0x0F00FFFF) | 0xB0B30000;
    
    //16 bit data frame, software slave management, internal slave select high, MSBFIRST, enable SPI, divide 36mhz peripheral clock by 2 for 18mbit, master mode, CPOL = 0, CPHA = 0
    SPI2_CR1 = 0b0000101101000100;
    //SPI2_CR1 = 0b0000101101111100;//TESTING slower clock that can be seen by logic analyzer
    SPI2_CR2 = 0b00000010;//DMA request on txe
    
    //DMA setup input setup
    //TODO
    
    //DMA setup output setup
    DMA_CPAR5 = (uint32_t)(&SPI2_DR);
    DMA_CMAR5 = (uint32_t)(outBuffer);
    //Med priority, 16 bit memory and peripheral transfers, memory increment, write to peripheral
    DMA_CCR5 = 0b0001010110010000;
    
    //TODO wait for positive edge from video mcu to signal it is ready
}

/*
bool SPIIO_cpu_full()//If out buffer is full
{
    uint16_t nextOutPointer;
    
    if (outPointer >= (SPIIO_BUFFER_SIZE - 1))
        nextOutPointer = 0;
    else
        ++nextOutPointer;
    
    return nextOutPointer == dmaOutPointer;
}

void SPIIO_cpu_push(uint16_t data)//Only call this if SPIIO_cpu_full() is false
{
    outBuffer[outPointer] = data;
    
    if (outPointer >= (SPIIO_BUFFER_SIZE - 1))
        outPointer = 0;
    else
        ++outPointer;
}

void SPIIO_cpu_flush()
{
    if (!(DMA_CCR5 & 1))//A dma transfer/flush is not currently in progress
    {
        DMA_CMAR5 = (uint32_t)(outBuffer + dmaOutPointer);
        dmaCount
    }
        
    //TODO provide blocking function and figure out way to handle slave select
    //DMA_CCR5 &= ~1;
    //DMA_CNDTR5 = outPointer;
    //DMA_CCR5 |= 1;//Enable dma transfer
    
    outPointer = 0;//TODO only reset this after dma finishes/allow pushes during flush
}
*/


bool SPIIO_cpu_full()//If out buffer is full
{
    return outPointer >= SPIIO_BUFFER_SIZE;
}

void SPIIO_cpu_push(uint16_t data)//Only call this if SPIIO_cpu_full() is false
{
    outBuffer[outPointer] = data;
    ++outPointer;
}

void SPIIO_cpu_flush()
{
    //TODO provide blocking function and figure out way to handle slave select
    DMA_CCR5 &= ~1;
    DMA_CNDTR5 = outPointer;
    DMA_CCR5 |= 1;//Enable dma transfer
    
    outPointer = 0;//TODO only reset this after dma finishes/allow pushes during flush
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

__attribute__ ((interrupt ("IRQ"))) void __ISR_DMA1_Channel5()
{
    //TODO set new value for dmaOutPointer here based on old value of dmaOutPointer and dmaCount
}

/*
__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI0()
{
    recievedEXTIInterrupt = true;//Got an interrupt
}*/
