#include "bluepill.h"
#include "spiio.h"

void printString(const char* string);

void main()
{
    //First thing to print should be "CPU MCU Initialized" (at 25, 32), then # of free basic bytes (at 1, 40)
    /*
    SPIBus_init_CPU();
    
    //TESTING
    uint8_t i = 0;
    while (true)
    {
        //__delayInstructions(50);//For stress testings
        __delayInstructions(4720299);//About 1 second
        SPIBus_transfer_CPU(i);
        
        ++i;
    }
    */
    
    //__delayInstructions(47202999);//TODO instead of delaying, have SPIIO_cpu_init block until video mcu is ready
    SPIIO_cpu_init();
    
    printString("CPU MCU Initialized\n");
    printString("????? basic bytes free :)\n");
    SPIIO_cpu_flush();
    
    __delayInstructions(4720299);//TESTING
    
    while (SPIIO_cpu_full()) { SPIIO_cpu_flush(); }
    SPIIO_cpu_push((5 << 9) | 300);
    SPIIO_cpu_push(200);
    SPIIO_cpu_push((5 << 9) | 150);
    SPIIO_cpu_push(100);
    SPIIO_cpu_flush();
    
    /*
    __delayInstructions(47202999);
    SPIIO_cpu_push((1 << 9) | 2);//TEST scrolling screen
    SPIIO_cpu_flush();
    __delayInstructions(47202999);
    */
    
    //while (true);//TESTING
    
    while (true)
    {
        /*
        __delayInstructions(4720299);//TESTING
        //__delayInstructions(4720);//TESTING
        //while (DMA_CNDTR5);//TESTING
        
        for (uint32_t i = 0; i < 64; ++i)
        {
            SPIIO_cpu_push(i);
        }
        SPIIO_cpu_flush();
        */
        
        /*
        __delayInstructions(47202999);//TESTING//Delay so that video mcu has time to process information (avoid overrun)
        
        uint32_t i = 0;
        while (i < 256)
        {
            if (SPIIO_cpu_full())
                SPIIO_cpu_flush();
            else
                SPIIO_cpu_push(i++ & 0xFF);
        }
        */
    }
    
    return;//Temporary
}

void printString(const char* string)
{
    char character = *string;
    if (character)
    {
        while (SPIIO_cpu_full()) { SPIIO_cpu_flush(); }
        
        SPIIO_cpu_push((2 << 9) | character);//First character is sent along with string write command
        
        while (true)
        {
            uint16_t command;
            ++string;
            character = *string;
            
            command = character;
            
            if (!character)
            {
                while (SPIIO_cpu_full()) { SPIIO_cpu_flush(); }
                SPIIO_cpu_push(command);
                break;
            }
            
            ++string;
            character = *string;
            
            command |= character << 8;
            
            while (SPIIO_cpu_full()) { SPIIO_cpu_flush(); }
            SPIIO_cpu_push(command);
            
            if (!character)
                break;
        }
    }
}
