#include <stdint.h>

#include "composite.h"
#include "softrenderer.h"
#include "vincent.h"//8x8 Font
#include "processing.h"
#include "spiio_video.h"
#include "ps2uart_video.h"

//NOTE: Unfortunatly, newlib takes up 100 bytes of space
//TODO get softrenderer operations to use dma for memory copying (ex scrolling, clearing, filling, etc)

//TODO add support for audio played on video mcu

//TODO after figuring out how much ram is left after stack/data for processing commands for cpu mcu, try to expand the resolution
volatile uint8_t ramFB[242][59];

const uint8_t JZJ[] =//My initials
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 
	0xff, 0xe0, 0x00, 0x08, 0x00, 0x80, 0x04, 0x00, 0x00, 0x08, 0x01, 0x00, 0x04, 0x00, 0x00, 0x08, 
	0x02, 0x00, 0x04, 0x00, 0x00, 0x08, 0x04, 0x00, 0x04, 0x00, 0x00, 0x08, 0x08, 0x00, 0x04, 0x00, 
	0x02, 0x08, 0x10, 0x01, 0x04, 0x00, 0x01, 0x10, 0x20, 0x00, 0x88, 0x00, 0x00, 0xe0, 0x7f, 0xc0, 
	0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void main()
{
    GPIOC_CRH = (GPIOC_CRH & 0xFF0FFFFF) | 0x00300000;//TESTING for led
    
    //Project information header
    SR_setCharacterRom(vincentFont);
    SR_setFrameBuffer((uint8_t*)ramFB);
    
    SR_drawText(1, 8,  "JZJBasic on Dual STM32F103C8T6's (20K+20K SRAM)");
    SR_drawText(1, 16, "Copyright (c) 2021 John Jekel");
    SR_drawText(1, 24, "MIT Licensed: https://gitlab.com/JZJisawesome/stm32basic");
    
    //My initials
    for (uint32_t i = 0; i < 16; ++i)
    {
        for (uint32_t j = 0; j < 6; ++j)
            ramFB[i + 8][j + 53] = JZJ[i*6 + j];
    }
    
    //Init code
    Composite_init((uint8_t*)ramFB);//First that way project information is displayed during initialization
    
    //TODO Do other init code here
    PS2UART_video_init();
    SPIIO_video_init();//Should be last thing to initialize
    
    SR_drawText(1, 32, "Video MCU Initialized : ");
    
    Processing_begin((uint8_t*)ramFB);//Never exits
}
