#include <stdint.h>
#include <stdbool.h>

#include "bluepill.h"
#include "vhal.h"
#include "spiio_cpu.h"
#include "basi.h"
#include "ps2uart_cpu.h"

//NOTE: Unfortunatly, newlib takes up 100 bytes of space
//TODO maybe move keyboard code/ascii translation to video mcu and send over serial?

void main()
{
    //TODO First thing to print should be "CPU MCU Initialized" (at 25, 32), then # of free basic bytes (at 1, 40)
    
    GPIOC_CRH = (GPIOC_CRH & 0xFF0FFFFF) | 0x00300000;//TESTING for led
    
    PS2UART_cpu_init();
    
    SPIIO_cpu_init();
    
    VHAL_drawText("CPU MCU Initialized\n");
    
    //TESTING
/*************************************************************/
    

    while (true);
    //TESTING end
/*************************************************************/
    
    BASIC_init();
    BASIC_begin();//Begin basic interpreter (never exits)
    
    
/*************************************************************/
    
    //TESTING
    /*
    VHAL_drawText("????? basic bytes free :)\n");
    //VHAL_flush();
    
    //__delayInstructions(47202990);//TESTING
    
    //GPIOC_BSRR = 1 << 13;
    VHAL_drawLineTo(300, 200);
    VHAL_drawLineTo(150, 100);
    VHAL_drawRectangle(10, 10);
    VHAL_drawTriangle(100, 50, 400, 250);
    VHAL_drawCircle(10);
    VHAL_drawTriangle_atPos(200, 0, 0, 250, 400, 250);
    VHAL_drawCircle_atPos(100, 100, 25);
    //GPIOC_BRR = 1 << 13;
    VHAL_flush();
    //VHAL_clear();
    
    */
    
    /*
    GPIOC_BSRR = 1 << 13;
    for (uint32_t i = 0; i < 256; ++i)
        VHAL_drawLine_atPos(0, 0, 400, i);
    
    VHAL_flush();
    GPIOC_BRR = 1 << 13;
    */
    
    /*
    __delayInstructions(47202999);
    SPIIO_cpu_push((1 << 9) | 2);//TEST scrolling screen
    SPIIO_cpu_flush();
    __delayInstructions(47202999);
    */
    //VHAL_setPos(225, 120);
    /*
    while (true)//TESTING
    {
        for (uint8_t radius = 0; radius < 100; ++ radius)
        {
            VHAL_clear();
            VHAL_drawCircle_atPos(radius + 10, 120, radius);
            VHAL_flush();
            __delayInstructions(400000);
        }
    }
    */
    //while (true);//TESTING
    //VHAL_clear();
    //VHAL_setPos(8, 8);
    //bool capital = false;
    //while (true)
    //{
    //    if (!PS2UART_empty())
    //    {
    //        char character = PS2UART_pop();
            /*
            char numString[4];
            numString[3] = 0x00;//Null byte
            numString[2] = '0' + (character % 10);//1s place
            numString[1] = '0' + ((character / 10) % 10);//10s place
            numString[0] = '0' + ((character / 100) % 10);//100s place
            VHAL_drawText(numString);
            VHAL_flush();
            */
       //     VHAL_drawChar(character);
       //     VHAL_flush();
            /*
            if ((character != 0xE0) && (character != 0xF0))
            {
                VHAL_drawChar(PS2_toAscii(character, capital));
                VHAL_flush();
                capital = !capital;
            }
            */
        //}
        //VHAL_flush();
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
        
        /*
        __delayInstructions(47202999);//TESTING//Delay so that video mcu has time to process information (avoid overrun)
        
        uint32_t i = 0;
        while (i < 256)
        {
            VHAL_drawChar(i++);
        }
        */
    //}
}
