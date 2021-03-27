#include "processing.h" 
#include "bluepill.h"
#include "softrenderer.h"
#include "fifo.h"
#include "spibus.h"

static fifo_t commandQueue;
static uint_fast16_t xPosition = 208;//Pixels
static uint_fast16_t yPosition = 32;//Pixels

static void processingLoop();
static void incrementCharacterPosition();

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
    /*
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
    */
    while (true)
    {
        if (FIFO_isEmpty(&commandQueue))
            //__wfi();
            ((void)0);//TESTING Debugging dies if this is wfi; change to wfi at end of development
        else
        {
            SPIBus_disableInterrupts_video();//Disable SPI2 ISR during pop
            uint16_t command = FIFO_pop(&commandQueue);
            SPIBus_enableInterrupts_video();//Enable SPI2 ISR
            
            if (command & (1 << 15))//Non character-write command
            {
                //TODO switch based on bits [15:9]
            }
            else//Character write command
            {
                //NOTE: Command bits [14:7] may have additional info that can be used
                
                char character = command & 0x7F;
                switch (character)
                {
                    //TODO ensure the special cases work correctly and have proper bounds checking
                    case 0x01://Move up
                    {
                        yPosition -= 8;
                        break;
                    }
                    case 0x02://Move left
                    {
                        xPosition -= 8;
                        break;
                    }
                    case 0x03://Move down
                    {
                        yPosition += 8;
                        break;
                    }
                    case 0x04://Move right
                    {
                        xPosition += 8;
                        break;
                    }
                    case 0x08://Backspace
                    {//TODO fix this
                        xPosition -= 8;
                        SR_drawCharByByte(xPosition / 8, yPosition, ' ');
                        break;
                    }
                    case '\t'://Tab
                    {
                        xPosition += 3 * 8;//3 spaces
                        incrementCharacterPosition();//Handle fourth space with increment code to deal with wrapping
                        break;
                    }
                    case '\n'://New line
                    case '\r'://Carriage return
                    {
                        xPosition = 8;
                    }//Fallthrough
                    case '\v'://Vertical tab
                    {
                        yPosition += 8;
                        
                        if (yPosition >= PROCESSING_LINES)
                            yPosition = 8;//So character is not cut off
                        
                        break;
                    }
                    case 0x0C://Form feed
                    {
                        //commented out for testing
                        //xPosition = 8;
                        //yPosition = 8;
                        break;
                    }
                    case 0x7F://Delete
                    {//TODO fix this
                        xPosition += 8;
                        SR_drawCharByByte(xPosition / 8, yPosition, ' ');
                        break;
                    }
                    default:
                    {
                        SR_drawCharByByte(xPosition / 8, yPosition, character);
                        incrementCharacterPosition();
                        
                        break;
                    }
                }
            }
        }
    }
}

static void incrementCharacterPosition()
{
    xPosition += 8;
    
    if ((xPosition / 8) >= PROCESSING_BYTES_PER_LINE)
    {
        xPosition = 8;//So character is not cut off
        yPosition += 8;
    }
    
    if (yPosition >= PROCESSING_LINES)
        yPosition = 8;//So character is not cut off
}

//TODO maybe replace this interrupt with DMA into the fifo?
__attribute__ ((interrupt ("IRQ"))) void __ISR_SPI2()
{
    //TODO handle data recieved when recieve buffer no longer empty
    
    if (!FIFO_isFull(&commandQueue))//Else command is lost
        FIFO_push(&commandQueue, SPIBus_recieve_video());//Reading will clear the RXNE flag
}
