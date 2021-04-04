#include "processing.h" 

#include <stdint.h>

#include "bluepill.h"
#include "softrenderer.h"
#include "spiio_video.h"

//TODO get softrenderer operations to use dma for memory copying (ex scrolling, clearing, filling, etc)

//Offsets from borders to avoid text being cut off
#define LEFT_BORDER 1//In bytes
#define RIGHT_BORDER 1//In bytes
#define TOP_BORDER 8//In lines/pixels
#define BOTTOM_BORDER 16//In lines/pixels

//TODO seperate out processing loop/switches from code that actually does stuff somehow

//TODO add support for audio played on video mcu

//Typedefs

typedef enum
{
    CHAR_WRITE = 0x00, SCREEN_OP = 0x01, STRING_WRITE = 0x02, CHAR_POS_SET = 0x03,
    LINE_DRAW = 0x04, HLINE_DRAW = 0x05, VLINE_DRAW = 0x06, POLY_DRAW = 0x07,//TODO implement (except for LINE_DRAW)
    CIRCLE_DRAW = 0x08, AUDIO_OP = 0x09//TODO implement
} commandMajor_t;
typedef enum {CLEAR_SCROP = 0x000, FILL_SCROP = 0x001, SCROLL_UP_SCROP = 0x002, SCROLL_DOWN_SCROP = 0x003} screenOp_t;//TODO implement
typedef enum
{
    STOP_ALL_AUOP = 0x000, START_ALL_AUOP = 0x100,//Useful for synchronizing both channels//TODO implement
    STOP_CHANNEL0_AUOP = 0x001, START_CHANNEL0_AUOP = 0x002, NOTE_QUEUE_CHANNEL0_AUOP = 0x003,//TODO implement
    STOP_CHANNEL1_AUOP = 0x102, START_CHANNEL1_AUOP = 0x102, NOTE_QUEUE_CHANNEL1_AUOP = 0x103//TODO implement
} audioOp_t;

//Static vars

static enum
{
    SINGLE, STRING_WRITE_MC, LINE_DRAW_MC, HLINE_DRAW_MC,
    VLINE_DRAW_MC, POLY_DRAW_MC, CIRCLE_DRAW_MC, AUDIO_OP_MC
    
} multiCommand = SINGLE;

static const uint8_t* frameBuffer;//Pointer to framebuffer

static uint_fast16_t characterX = 25;//In bytes
static uint_fast16_t characterY = 32;//In lines/pixels
static uint_fast16_t scratch[8];

//static uint_fast16_t xPosition = 200;//Pixels
//static uint_fast16_t yPosition = 32;//Pixels

//Functions

static void processingLoop();
static void handleSingleCommand(uint16_t command);
static void handleCharacter(char character);
static void screenOperation(screenOp_t operation);
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
                case SINGLE://Not in the middle of a multi command command; parse normally
                {
                    handleSingleCommand(command);
                    break;
                }
                case STRING_WRITE_MC://String write (more efficient than individual character writes)
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
                            multiCommand = SINGLE;//Exit multi command
                    }
                    else
                        multiCommand = SINGLE;//Exit multi command
                    
                    break;
                }
                case LINE_DRAW_MC:
                {
                    if (scratch[4] == 3)
                    {
                        SR_drawLine(scratch[0], scratch[1], scratch[2], command & 0x1FF);
                        multiCommand = SINGLE;//Exit multi command
                    }
                    else
                    {
                        scratch[scratch[4]] = command & 0x1FF;
                        ++scratch[4];
                    }
                    break;
                }
                case HLINE_DRAW_MC:
                {
                    break;//TODO
                }
                case VLINE_DRAW_MC:
                {
                    break;//TODO
                }
                case POLY_DRAW_MC:
                {
                    break;//TODO
                }
                case CIRCLE_DRAW_MC:
                {
                    if (scratch[4] == 2)
                    {
                        SR_drawCircle(scratch[0], scratch[1], command & 0x1FF);
                        multiCommand = SINGLE;//Exit multi command
                    }
                    else
                    {
                        scratch[1] = command & 0x1FF;//y
                        ++scratch[4];
                    }   
                    
                    break;//TODO
                }
                case AUDIO_OP_MC:
                {
                    break;//TODO
                }
                /*
                case 5://Line draw from current position
                {
                    uint32_t destYPos = command & 0x1FF;
                    SR_drawLine(xPosition, yPosition, scratch[0], destYPos);
                    multiCommand = SINGLE;
                    
                    //Set new current location
                    xPosition = scratch[0];
                    yPosition = destYPos;
                    break;
                }
                case 8://Rectangle from current position
                {
                    SR_drawRectangle(xPosition, yPosition, scratch[0], command & 0x1FF);
                    multiCommand = SINGLE;
                    break;
                }
                case 9://Triangle from current position
                {
                    if (scratch[4] == 3)
                    {
                        SR_drawTriangle(xPosition, yPosition, scratch[0], scratch[1], scratch[2], command & 0x1FF);
                        multiCommand = SINGLE;//End this command
                    }
                    else
                    {
                        scratch[scratch[4]] = command & 0x1FF;//Save coords
                        scratch[4] += 1;
                    }
                    
                    break;
                }
                */
            }
        }
    }
}

static void handleSingleCommand(uint16_t command)
{
    commandMajor_t commandMajor = (commandMajor_t)(command >> 9);
    
    switch (commandMajor)//Look at bits [15:9] for the command to execute
    {
        case CHAR_WRITE://Character write
        {//0 was chosen for CHAR_WRITE because it means that a character can just be sent as is from the CPU
            //NOTE: Command bits [9:7] may have additional info that can be used
            handleCharacter(command & 0x7F);
            break;
        }
        case SCREEN_OP://Screen operation
        {
            screenOperation((screenOp_t)(command & 0x1FF));
            break;
        }
        case STRING_WRITE://String write (more efficient than individual character writes)
        {//Ends when a null byte is encountered
            char character = command & 0xFF;
            if (character)
            {
                multiCommand = STRING_WRITE_MC;
                handleCharacter(character);
            }
            break;
        }
        case CHAR_POS_SET://Set character positions
        {
            //WARNING no bounds checking
            characterX = command & 0x0F;
            characterY = ((command >> 4) & 0x0F) * 8;//8 pixels/lines per character coordinate
            break;
        }
        case LINE_DRAW:
        {
            scratch[0] = command & 0x1FF;//x
            multiCommand = LINE_DRAW_MC;
            scratch[4] = 1;//scratch[4] contains next coordinate to get
            break;
        }
        case HLINE_DRAW:
        {
            break;//TODO
        }
        case VLINE_DRAW:
        {
            break;//TODO
        }
        case POLY_DRAW:
        {
            break;//TODO
        }
        case CIRCLE_DRAW:
        {
            scratch[0] = command & 0x1FF;//x0
            multiCommand = CIRCLE_DRAW_MC;
            scratch[4] = 1;//scratch[4] contains next data thing to get
            break;//TODO
        }
        case AUDIO_OP:
        {
            break;//TODO
        }
        /*
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
        }*/
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
        {//TODO fix this if at beginning of line
            --characterX;
            SR_drawCharByByte_OW(characterX, characterY, ' ');//Overwrite with a space
            break;
        }
        case '\t'://Tab
        {
            characterX += 3;//3 spaces
            incrementCharacterPosition();//Handle fourth space with increment code to deal with wrapping
            break;
        }
        case '\n'://New line
        case '\r'://Carriage return
        {
            characterX = LEFT_BORDER;
        }//Fallthrough
        case '\v'://Vertical tab
        {
            characterY += 8;
            
            if (characterY >= (PROCESSING_LINES - BOTTOM_BORDER))
            {
                SR_scrollUp(8);//Scroll screen back
                characterY -= 8;
            }
            
            break;
        }
        case 0x0C://Form feed
        {//TODO is this what this should do?
            characterX = LEFT_BORDER;
            characterY = TOP_BORDER;
            break;
        }
        case 0x7F://Delete
        {//TODO fix this
            ++characterX;
            SR_drawCharByByte_OW(characterX, characterY, ' ');
            break;
        }
        default:
        {
            SR_drawCharByByte_OW(characterX, characterY, character);
            incrementCharacterPosition();
            
            break;
        }
    }
}

static void incrementCharacterPosition()
{
    ++characterX;
    
    if (characterX >= (PROCESSING_BYTES_PER_LINE - (RIGHT_BORDER / 8)))
    {
        characterX = LEFT_BORDER;//Wrap
        characterY += 8;
    }
    
    if (characterY >= (PROCESSING_LINES - BOTTOM_BORDER))
    {
        SR_scrollUp(8);//Scroll screen back
        characterY -= 8;//Push position back
    }
}

static void screenOperation(screenOp_t operation)
{
    switch (operation)
    {
        case CLEAR_SCROP:
        {
            SR_clear();
            break;
        }
        case FILL_SCROP:
        {
            SR_fill();
            break;
        }
        case SCROLL_UP_SCROP:
        {
            SR_scrollUp(8);
            break;
        }
        case SCROLL_DOWN_SCROP:
        {
            SR_scrollDown(8);
            break;
        }
    }
}
