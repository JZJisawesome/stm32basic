#include "spibus.h"
#include "bluepill.h"

void main()
{
    //First thing to print should be "CPU MCU Initialized" (at 25, 32), then # of free basic bytes (at 1, 40)
    SPIBus_init_CPU();
    
    //TESTING
    while (true)
    {
        SPIBus_transfer_CPU('J');
        __delayInstructions(47202987);//About 1 second
    }
    
    return;//Temporary
}
