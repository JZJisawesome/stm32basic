#include "composite.h"
#include "softrenderer.h"
#include "vincent.h"//8x8 Font

//TODO after figuring out how much ram is left after stack/data for processing commands for cpu mcu, try to expand the resolution
uint8_t ramFB[242][59];

void main()
{
    SR_setCharacterRom(vincentFont);
    SR_setFrameBuffer((uint8_t*)ramFB);
    Composite_init((uint8_t*)ramFB);
    
    SR_drawText(1, 8, "Hello World!");
    
    //TODO handle commands from second spi peripheral
    
    return;
}
