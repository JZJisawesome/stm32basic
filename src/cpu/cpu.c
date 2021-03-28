#include "spibus.h"
#include "bluepill.h"
#include "spiio.h"

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
    
    __delayInstructions(47202999);//TODO instead of delaying, have SPIIO_cpu_init block until video mcu is ready
    SPIIO_cpu_init();
    
    const char* testString = "CPU MCU Initialized\n";
    
    while (*testString != 0)
    {
        if (SPIIO_cpu_full())
            SPIIO_cpu_flush();
        else
            SPIIO_cpu_push(*(testString++));
    }
    SPIIO_cpu_flush();
    
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
        
        
        __delayInstructions(47202999);//TESTING//Delay so that video mcu has time to process information (avoid overrun)
        
        uint32_t i = 0;
        while (i < 256)
        {
            if (SPIIO_cpu_full())
                SPIIO_cpu_flush();
            else
                SPIIO_cpu_push(i++ & 0xFF);
        }
    }
    
    return;//Temporary
}
