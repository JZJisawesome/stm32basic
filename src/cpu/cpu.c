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
    
    SPIIO_cpu_init();
    
    while (true)
    {
        __delayInstructions(4720299);
        for (uint32_t i = 0; i < 64; ++i)
        {
            SPIIO_cpu_push(i);
        }
        SPIIO_cpu_flush();
    }
    
    return;//Temporary
}
