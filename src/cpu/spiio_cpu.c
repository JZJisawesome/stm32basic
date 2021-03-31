#include "spiio_cpu.h"
#include "bluepill.h"

//TODO if possible do pointer chacing like in spiio_video instead of two buffers that are half the size
//More efficient that way
//Ex. maybe have pointer chase dma counter and consider both going past it and the 64 entry threshold as full
//That way dma transfers don't need to wrap maybe?
//IDK

#define HALF_BUFFER_SIZE (SPIIO_CPU_BUFFER_SIZE / 2)

//Static vars
static volatile uint16_t buffer[SPIIO_CPU_BUFFER_SIZE];//Read by DMA
static uint16_t pointer = 0;//Software pointer (not DMA pointer)
static bool dmaBufferFirstHalf = true;//Next half of buffer to transfer will be the first half

//Functions

void SPIIO_cpu_init()
{
    //Set PB15, PB13 (MOSI, SCK) as AF outputs; PB12 (NSS) as gpio output; PB14 as gpio input pulldown
    GPIOB_BSRR = 1 << 12;//PB12 (NSS should start high)//NOTE: PB14 is pulldown by default
    GPIOB_CRH = (GPIOB_CRH & 0x0000FFFF) | 0xB8B30000;
    
    //16 bit data frame, software slave management, internal slave select high, MSBFIRST, enable SPI, divide 36mhz peripheral clock by 2 for 18mbit, master mode, CPOL = 0, CPHA = 0
    //SPI2_CR1 = 0b0000101101000100;
    SPI2_CR1 = 0b0000101101111100;//TESTING slower clock that can be seen by logic analyzer
    SPI2_CR2 = 0b00000010;//DMA request on txe
    
    //DMA setup output setup
    DMA_CPAR5 = (uint32_t)(&SPI2_DR);
    //Low (1/4) priority, 16 bit memory and peripheral transfers, memory increment, write to peripheral
    DMA_CCR5 = 0b0000010110010000;
    
    while (!(GPIOB_IDR & (1 << 14)));//Wait for video mcu to bring PB14 high
    GPIOB_BRR = 1 << 12;//Now bring NSS (PB12) low 
}

bool SPIIO_full()//If out buffer is full
{
    if (dmaBufferFirstHalf)
        return pointer >= HALF_BUFFER_SIZE;
    else
        return pointer >= SPIIO_CPU_BUFFER_SIZE;
}

void SPIIO_push(uint16_t data)//Only call this if SPIIO_full() is false (or after SPIIO_smartBlockingFlush())
{
    buffer[pointer] = data;
    ++pointer;
}

void SPIIO_flush()
{
    if (DMA_CNDTR5 == 0)//Transfer not currently occuring
    {
        uint16_t offset = dmaBufferFirstHalf ? 0 : HALF_BUFFER_SIZE;
        uint16_t amountToTransfer = pointer - offset;
        
        DMA_CCR5 &= ~1;//Disable DMA channel while modifying stuffs
        DMA_CMAR5 = (uint32_t)(buffer + offset);
        DMA_CNDTR5 = amountToTransfer;
        DMA_CCR5 |= 1;//Enable and start dma transfer
        
        pointer = dmaBufferFirstHalf ? HALF_BUFFER_SIZE : 0;//Set pointer to other half of buffer
        dmaBufferFirstHalf = !dmaBufferFirstHalf;
    }
}
