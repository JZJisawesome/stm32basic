#include "bluepill.h"
#include "vhal.h"
#include "spiio_cpu.h"

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
    
    GPIOC_CRH = (GPIOC_CRH & 0xFF0FFFFF) | 0x00300000;
    
    //__delayInstructions(47202999);//TODO instead of delaying, have SPIIO_cpu_init block until video mcu is ready
    SPIIO_cpu_init();
    
    VHAL_drawText("CPU MCU Initialized\n");
    VHAL_drawText("????? basic bytes free :)\n");
    //VHAL_flush();
    
    //__delayInstructions(47202990);//TESTING
    
    VHAL_drawLineTo(300, 200);
    VHAL_drawLineTo(150, 100);
    VHAL_drawRectangle(10, 10);
    VHAL_drawTriangle(100, 50, 400, 250);
    VHAL_drawTriangle_atPos(200, 0, 0, 250, 400, 250);
    VHAL_drawCircle_atPos(100, 100, 25);
    VHAL_flush();
    //VHAL_clear();
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
    
    //while (true);//TESTING
    
    while (true)
    {
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
    }
    
    return;
}
