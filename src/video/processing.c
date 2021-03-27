#include "processing.h" 
#include "bluepill.h"
#include "softrenderer.h"
#include "fifo.h"
#include "spibus.h"

static fifo_t commandQueue;

static void processingLoop();

void Processing_begin()
{
    processingLoop();
}

static void processingLoop()
{
    /*
    int i = 0;
    while (true)
    {
        SR_drawCharByByte(1 + i, 44, SPIBus_recieve_video());
        __delayInstructions(4720299);//About 0.1s//TESTING
        ++i;
    }
    */
    int i = 0;
    while (true)
    {
        //TODO handle commands from second spi peripheral
        if (!FIFO_isEmpty(&commandQueue))
        {
            SPIBus_disableInterrupts_video();//Disable SPI2 ISR during pop
            char data = FIFO_pop(&commandQueue);
            SPIBus_enableInterrupts_video();//Enable SPI2 ISR
            
            SR_drawCharByByte(1 + i, 44, data);
            ++i;
        }
    }
}

//TODO maybe replace this interrupt with DMA into the fifo?
__attribute__ ((interrupt ("IRQ"))) void __ISR_SPI2()
{
    //TODO handle data recieved when recieve buffer no longer empty
    
    if (!FIFO_isFull(&commandQueue))//Else command is lost
        FIFO_push(&commandQueue, SPIBus_recieve_video());//Reading will clear the RXNE flag
}
