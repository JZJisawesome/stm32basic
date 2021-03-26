
#include "composite.h"
#include "softrenderer.h"

//TODO after figuring out how much ram is left after stack/data for processing commands for cpu mcu, try to expand the resolution
uint8_t ramFB[242][59];

void main()
{
    Composite_init((uint8_t*)ramFB);
    
    //TODO handle commands from second spi peripheral
    
    return;
}
