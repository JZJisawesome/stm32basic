#include "spibus.h"

#include "bluepill.h"

//TODO use crc functionality to ensure no errors

//Init functions

void SPIBus_init_video()
{
    //Set PB14 (MISO) as AF output
    //TODO do in one operation to avoid glitches
    GPIOB_CRH &= 0xF0FFFFFF;
    GPIOB_CRH |= 0x0B000000;
    
    //16 bit data frame, hardware slave management, MSBFIRST, enable SPI, slave mode, CPOL = 0, CPHA = 0
    SPI2_CR1 = 0b0000100001000000;
    
    //Enable interrupts
    NVIC_ISER1 = 1 << 4;//Enable SPI2 ISR
    SPI2_CR2 |= 1 << 6;//Enable RXNE interrupt
}

void SPIBus_init_CPU()
{
    //Set PB15, PB13 (MOSI, SCK) as AF outputs; PB12 as gpio output
    //TODO do in one operation to avoid glitches
    GPIOB_CRH &= 0x0F00FFFF;
    GPIOB_CRH |= 0xB0B30000;
    
    //16 bit data frame, software slave management, internal slave select high, MSBFIRST, enable SPI, master mode, divide 36mhz peripheral clock by 2 for 18mbit, CPOL = 0, CPHA = 0
    SPI2_CR1 = 0b0000101101000100;
    //SPI2_CR1 = 0b0000101101111100;//TESTING slower clock that can be seen by logic analyzer
}

void SPIBus_transfer_CPU(uint16_t data)
{
    GPIOB_BRR = (1 << 12);
    SPI2_DR = data;
    
    while (SPI2_SR & (1 << 1));//Wait for txe
    while (SPI2_SR & (1 << 7));//Wait for bsy
    
    GPIOB_BSRR = (1 << 12);
}

//Stuff that is useful for the fifo

void SPIBus_disableInterrupts_video()
{
    NVIC_ICER1 = 1 << 4;//Disable SPI2 ISR during pop
}

void SPIBus_enableInterrupts_video()
{
    NVIC_ISER1 = 1 << 4;//Enable SPI2 ISR
}
