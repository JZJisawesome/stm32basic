#include "processing.h" 
#include "bluepill.h"
#include "softrenderer.h"
#include "spiio_video.h"

//TODO seperate out processing loop/switches from code that actually does stuff somehow

//TODO add support for audio played on video mcu

static const uint8_t* frameBuffer;//Pointer to framebuffer
static uint32_t multiCommand = 0;//0 Indicates not a multi command, other numbers indicate other things

static uint_fast16_t xPosition = 200;//Pixels
static uint_fast16_t yPosition = 32;//Pixels
static uint_fast16_t scratch[8];

static void processingLoop();
static void handleSingleCommand(uint16_t command);
static void handleCharacter(char character);
static void screenOperation(uint16_t operation);
static void incrementCharacterPosition();

void Processing_begin(const uint8_t* fb)
{
    //Store pointer to framebuffer
    frameBuffer = fb;
    
    processingLoop();
}

static void processingLoop()
{
    while (true)
    {
        if (SPIIO_empty())
            //__wfi();
            ((void)0);//TESTING Debugging dies if this is wfi; change to wfi at end of development if possible
        else
        {
            uint16_t command = SPIIO_pop();
            
            switch (multiCommand)
            {
                case 0://Not in the middle of a multi command command; parse normally
                {
                    handleSingleCommand(command);
                    break;
                }
                case 2://String write (more efficient than individual character writes)
                {
                    char character = command & 0xFF;//Low 8 bits
                    if (character)//Check for null byte
                    {
                        handleCharacter(character);
                        
                        character = command >> 8;//Upper 8 bits
                        if (character)//Check for null byte
                        {
                            handleCharacter(character);
                        }
                        else
                            multiCommand = 0;
                    }
                    else
                        multiCommand = 0;
                    
                    break;
                }
                case 5://Line draw from current position
                {
                    uint32_t destYPos = command & 0x1FF;
                    SR_drawLine(xPosition, yPosition, scratch[0], destYPos);
                    multiCommand = 0;
                    
                    //Set new current location
                    xPosition = scratch[0];
                    yPosition = destYPos;
                    break;
                }
                case 8://Rectangle from current position
                {
                    SR_drawRectangle(xPosition, yPosition, scratch[0], command & 0x1FF);
                    multiCommand = 0;
                    break;
                }
                case 9://Triangle from current position
                {
                    if (scratch[4] == 3)
                    {
                        SR_drawTriangle(xPosition, yPosition, scratch[0], scratch[1], scratch[2], command & 0x1FF);
                        multiCommand = 0;//End this command
                    }
                    else
                    {
                        scratch[scratch[4]] = command & 0x1FF;//Save coords
                        scratch[4] += 1;
                    }
                    
                    break;
                }
            }
        }
    }
}

static void handleSingleCommand(uint16_t command)
{
    switch (command >> 9)//Look at bits [15:9] for the command to execute
    {
        case 0://Character write
        {//0 was chosen because it means that a character can just be sent as is from the CPU
            //NOTE: Command bits [9:7] may have additional info that can be used
            handleCharacter(command & 0x7F);
            break;
        }
        case 1://Screen operation
        {
            screenOperation(command & 0x1FF);
            break;
        }
        case 2://String write (more efficient than individual character writes)
        {//Ends when a null byte is encountered
            char character = command & 0xFF;
            if (character)
            {
                multiCommand = 2;
                handleCharacter(character);
            }
            break;
        }
        case 3://Set x position
        {
            xPosition = command & 0x1FF;
            break;
        }
        case 4://Set y position
        {
            yPosition = command & 0x1FF;
            break;
        }
        case 5://Line draw from current position
        {
            scratch[0] = command & 0x1FF;//x destination
            multiCommand = 5;
            break;
        }
        case 6://Horizontal line draw from current position
        {
            uint16_t count = command & 0x1FF;
            SR_drawHLine(xPosition, yPosition, count);
            xPosition += count;
            break;
        }
        case 7://Vertical line draw from current position
        {
            uint16_t count = command & 0x1FF;
            SR_drawVLine(xPosition, yPosition, count);
            yPosition += count;
            break;
        }
        case 8://Rectangle from current position
        {
            scratch[0] = command & 0x1FF;//x count
            multiCommand = 8;
            break;
        }
        case 9://Triangle from current position
        {
            scratch[0] = command & 0x1FF;//x1
            scratch[4] = 1;//Current step of fetching triangle coordinates
            multiCommand = 9;
            break;
        }
        case 10:
        {
            SR_drawCircle(xPosition, yPosition, command & 0x1FF);
            break;
        }
        default:
        {
            break;
        }
    }
}

static void handleCharacter(char character)
{
    switch (character)
    {
        //TODO ensure the special cases work correctly and have proper bounds checking
        case 0x00:
        {
            //Just ignore a null character
            break;
        }
        /*
        //TODO make these into their own proper commands
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
        */
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
            xPosition = 8;
            yPosition = 8;
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

static void screenOperation(uint16_t operation)
{
    switch (operation)
    {
        case 0:
        {
            SR_clear();
            break;
        }
        case 1:
        {
            SR_fill();
            break;
        }
        case 2:
        {
            SR_scrollUp(8);
            break;
        }
        case 3:
        {
            SR_scrollDown(8);
            break;
        }
    }
}
