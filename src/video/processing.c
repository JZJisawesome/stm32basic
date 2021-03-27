#include "processing.h" 
#include "bluepill.h"
#include "softrenderer.h"
#include "fifo.h"
#include "spibus.h"
#include "spiio.h"

static uint_fast16_t xPosition = 200;//Pixels
static uint_fast16_t yPosition = 32;//Pixels

static void processingLoop();
static void handleCharacter(char character);
static void incrementCharacterPosition();

void Processing_begin()
{
    processingLoop();
}

static void processingLoop()
{
    while (true)
    {
        if (SPIIO_video_empty())
            //__wfi();
            ((void)0);//TESTING Debugging dies if this is wfi; change to wfi at end of development
        else
        {
            uint16_t command = SPIIO_video_pop();
            
            switch (command >> 9)//Look at bits [15:9] for the command to execute
            {
                case 0://0 was chosen because it means that a character can just be sent as is from the CPU
                {
                    //NOTE: Command bits [9:7] may have additional info that can be used
                    handleCharacter(command & 0x7F);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

static void handleCharacter(char character)
{
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
