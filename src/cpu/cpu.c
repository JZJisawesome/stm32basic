#include "spibus.h"
#include "bluepill.h"

void main()
{
    //First thing to print should be "CPU MCU Initialized" (at 25, 32), then # of free basic bytes (at 1, 40)
    SPIBus_init_CPU();
    
    SPIBus_transfer_CPU(0xdcba);
    
    return;//Temporary
}
