#include "spiio.h"

#include "bluepill.h"

//Static variables

static volatile uint16_t inBuffer[SPIIO_BUFFER_SIZE];
static volatile uint16_t outBuffer[SPIIO_BUFFER_SIZE];
static volatile uint16_t inPointer = 0;//CPU's pointer
static volatile uint16_t outPointer = 0;//CPU's pointer

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
    
    //Enable interrupts
    //NVIC_ISER1 = 1 << 4;//Enable SPI2 ISR
    //SPI2_CR2 |= 1 << 6;//Enable RXNE interrupt
    
    //TODO signal to cpu with positive edge that video is ready
}

bool SPIIO_video_empty()//If in buffer is empty
{
    return inPointer == (SPIIO_BUFFER_SIZE - DMA_CNDTR4);
}

uint16_t SPIIO_video_pop()
{
    return inBuffer[inPointer++];
}

//Functions for cpu

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
