#include "composite.h"
#include "softrenderer.h"
#include "vincent.h"//8x8 Font
#include "processing.h"
#include "spiio_video.h"

//TODO after figuring out how much ram is left after stack/data for processing commands for cpu mcu, try to expand the resolution
volatile uint8_t ramFB[242][59];

void drawCircle(uint32_t x, uint32_t y, uint32_t radius);

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
    //Project information header
    SR_setCharacterRom(vincentFont);
    SR_setFrameBuffer((uint8_t*)ramFB);
    
    SR_drawText(1, 8,  "JZJBasic on Dual STM32F103C8T6's");
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
    
    SPIIO_video_init();//Should be last thing to initialize
    
    SR_drawText(1, 32, "Video MCU Initialized : ");
    //drawCircle(200, 120, 50);
    
    Processing_begin((uint8_t*)ramFB);//Never exits
}

void drawCircle(uint32_t x, uint32_t y, uint32_t radius)
{
    //https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    //https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
    //TODO figure out why things are ovals
    //TODO understand how this works; comments below are from me
    //Figure out //?
    
    assert(radius > 0);
    
    //Coordinates relative to 0, 0
    uint32_t currentX = radius;//?
    uint32_t currentY = 0;//?
    
    //Draw points along axes
    SR_drawPoint(x + currentX, y);//Rightmost pixel
    SR_drawPoint(x - currentX, y);//Leftmost pixel
    SR_drawPoint(y, x + currentX);//Highest pixel
    SR_drawPoint(y, x - currentX);//Lowest pixel
    
    int32_t midpoint = 1 - radius;//?
    while (currentX > currentY)//?
    {
        //Determine coordinates to plot in octant 1
        
        ++currentY;//We move up one pixel each time
        
        if (midpoint <= 0)//?
            midpoint += (2 * currentY) + 1;//?
        else
        {
            --currentX;//?
            midpoint += (2 * currentY) - (2 * currentX) + 1;//?
        }
        
        if (currentX < currentY)//?
            break;
        
        //Draw points based on new coordinates
        
        SR_drawPoint(x + currentX, y + currentY);//Octant 1
        SR_drawPoint(x - currentX, y + currentY);//Octant 4
        SR_drawPoint(x + currentX, y - currentY);//Octant 8
        SR_drawPoint(x - currentX, y - currentY);//Octant 5
        
        //The if statement here isn't needed, but it saves time; TODO figure out why
        //Something to do with not needing to generate points that overlap
        if (currentX != currentY)//?
        {
            SR_drawPoint(x + currentY, y + currentX);//Octant 2
            SR_drawPoint(x - currentY, y + currentX);//Octant 3
            SR_drawPoint(x + currentY, y - currentX);//Octant 7
            SR_drawPoint(x - currentY, y - currentX);//Octant 6
        }
    }
    
    //uint32_t r_2 = radius * radius;
    //SR_drawPoint(
}
