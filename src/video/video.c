
#include "composite.h"

uint8_t ramFB[242][59];

void main()
{
    Composite_init((uint8_t*)ramFB);
    
    //TODO handle commands from second spi peripheral
    
    return;
}
