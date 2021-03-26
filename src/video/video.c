#include "composite.h"
#include "softrenderer.h"
#include "vincent.h"//8x8 Font

//TODO after figuring out how much ram is left after stack/data for processing commands for cpu mcu, try to expand the resolution
uint8_t ramFB[242][59];

void main()
{
    SR_setCharacterRom(vincentFont);
    SR_setFrameBuffer((uint8_t*)ramFB);
    
    SR_drawText(1, 8,  "JZJBasic on Dual STM32F103C8T6's");
    SR_drawText(1, 16, "Copyright (c) 2021 John Jekel");
    SR_drawText(1, 24, "MIT Licensed: https://gitlab.com/JZJisawesome/stm32basic");
    
    Composite_init((uint8_t*)ramFB);
    
    //Do init code here
    
    SR_drawText(1, 32, "Video MCU Initialized : ");
    
    
    //TODO handle commands from second spi peripheral
    
    return;
}
