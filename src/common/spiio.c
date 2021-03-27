#include "spiio.h"

#include "bluepill.h"

static volatile void recievedEXTIInterrupt = false;

//Common functions

void SPIIO_extiInit()
{
    //Set PA1 as push-pull gpio output and leave PA0 as input
    GPIOA_CRL = (GPIOA_CRL & 0xFFFFFF0F) | 0x00000030;
    //TODO setup external interrupts
}

//Functions for video

void SPIIO_video_spiInit()
{
    //Set PB14 (MISO) as AF output
    GPIOB_CRH = (GPIOB_CRH & 0xF0FFFFFF) | 0x0B000000;
    
    //16 bit data frame, hardware slave management, MSBFIRST, enable SPI, slave mode, CPOL = 0, CPHA = 0
    SPI2_CR1 = 0b0000100001000000;
    
    //Enable interrupts
    NVIC_ISER1 = 1 << 4;//Enable SPI2 ISR
    SPI2_CR2 |= 1 << 6;//Enable RXNE interrupt
    
    //TODO signal to cpu with positive edge that video is ready
}

//Functions for cpu

void SPIIO_cpu_spiInit()
{
    while (!videoReady);
}

//Interrupts
__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI0()
{
    recievedEXTIInterrupt = true;//Got an interrupt
}
