#include "processing.h" 

#include <stdint.h>
#include <assert.h>

#include "bluepill.h"
#include "communication_defs.h"
#include "softrenderer.h"
#include "spiio_video.h"
#include "audio.h"

//TODO get softrenderer operations to use dma for memory copying (ex scrolling, clearing, filling, etc)

//Offsets from borders to avoid text being cut off
#define LEFT_BORDER 1//In bytes
#define RIGHT_BORDER 1//In bytes
#define TOP_BORDER 8//In lines/pixels
#define BOTTOM_BORDER 16//In lines/pixels

//TODO seperate out processing loop/switches from code that actually does stuff somehow

//TODO add support for audio played on video mcu

//TODO add support for relative character position moving

//Static vars

static const uint8_t* frameBuffer;//Pointer to framebuffer

static uint_fast16_t characterX = 25;//In bytes
static uint_fast16_t characterY = 32;//In lines/pixels

//Functions

static void processingLoop();
static void handleCommand(uint16_t command);
static void handleCharacter(char character);
static void incrementCharacterPosition();
static void screenOperation(spiScreenOp_t operation);
static void audioOperation(spiAudioOp_t operation);

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
            handleCommand(SPIIO_pop());
    }
}

static void handleCommand(uint16_t command)
{
    spiMajorCommand_t commandMajor = (spiMajorCommand_t)(command >> 9);
    
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
            screenOperation((spiScreenOp_t)(command & 0x1FF));
            break;
        }
        case STRING_WRITE://String write (more efficient than individual character writes)
        {//Ends when a null byte is encountered
            char character = command & 0x7F;
            if (character)
            {
                handleCharacter(character);
                
                bool nullByteEncountered = false;
                while (!nullByteEncountered)
                {
                    while (SPIIO_empty());//Wait for data
                    uint16_t data = SPIIO_pop();
                    
                    character = data & 0x7F;
                    if (character != 0x00)
                    {
                        handleCharacter(character);
                        
                        character = (data >> 8) & 0x7F;
                        if (character != 0x00)
                            handleCharacter(character);
                        else
                            nullByteEncountered = true;
                    }
                    else
                        nullByteEncountered = true;
                }
            }
            
            break;
        }
        case CHAR_POS_SET://Set character positions
        {
            //WARNING no bounds checking
            characterX = command & 0xFF;
            while (SPIIO_empty());//Wait for data
            characterY = (SPIIO_pop() & 0xFF) * 8;
            break;
        }
        case LINE_DRAW:
        {
            uint32_t x0 = command & 0x1FF;
            uint32_t y0, x1, y1;
            
            while (SPIIO_empty());//Wait for data
            y0 = SPIIO_pop() & 0x1FF;
            while (SPIIO_empty());//Wait for data
            x1 = SPIIO_pop() & 0x1FF;
            while (SPIIO_empty());//Wait for data
            y1 = SPIIO_pop() & 0x1FF;
            
            SR_drawLine(x0, y0, x1, y1);
            break;
        }
        case HLINE_DRAW:
        {
            uint32_t x = command & 0x1FF;
            uint32_t y, length;
            
            while (SPIIO_empty());//Wait for data
            y = SPIIO_pop() & 0x1FF;
            while (SPIIO_empty());//Wait for data
            length = SPIIO_pop() & 0x1FF;
            
            SR_drawHLine(x, y, length);
            break;
        }
        case VLINE_DRAW:
        {
            uint32_t x = command & 0x1FF;
            uint32_t y, length;
            
            while (SPIIO_empty());//Wait for data
            y = SPIIO_pop() & 0x1FF;
            while (SPIIO_empty());//Wait for data
            length = SPIIO_pop() & 0x1FF;
            
            SR_drawVLine(x, y, length);
            break;
        }
        case POLY_DRAW:
        {
            uint32_t amount = command & 0x1FF;//How many points to connect
            
            assert(amount > 1);
            
            uint32_t currentX, currentY, previousX, previousY, firstX, firstY;
            
            //Handle the first point ourselves
            while (SPIIO_empty());//Wait for data
            firstX = SPIIO_pop() & 0x1FF;
            previousX = firstX;
            while (SPIIO_empty());//Wait for data
            firstY = SPIIO_pop() & 0x1FF;
            previousY = firstY;
            --amount;
            
            for (uint32_t i = 0; i < amount; ++i)
            {
                while (SPIIO_empty());//Wait for data
                currentX = SPIIO_pop() & 0x1FF;
                while (SPIIO_empty());//Wait for data
                currentY = SPIIO_pop() & 0x1FF;
                
                SR_drawLine(previousX, previousY, currentX, currentY);
                
                previousX = currentX;
                previousY = currentY;
            }
            
            SR_drawLine(previousX, previousY, firstX, firstY);//Finish the polygon
            
            break;
        }
        case CIRCLE_DRAW:
        {
            uint32_t x = command & 0x1FF;
            uint32_t y, radius;
            
            while (SPIIO_empty());//Wait for data
            y = SPIIO_pop() & 0x1FF;
            while (SPIIO_empty());//Wait for data
            radius = SPIIO_pop() & 0x1FF;
            
            SR_drawCircle(x, y, radius);
            
            break;
        }
        case AUDIO_OP:
        {
            audioOperation((spiAudioOp_t)(command & 0x1FF));
            break;//TODO
        }
        case CHAR_RELX_POS_SET:
        {
            //TODO keep in bounds
            int8_t offsetX = (int8_t)(command & 0xFF);
            characterX += offsetX;
            break;
        }
        case CHAR_RELY_POS_SET:
        {
            //TODO keep in bounds
            int8_t offsetY = (int8_t)(command & 0xFF);
            characterY += offsetY * 8;
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

static void screenOperation(spiScreenOp_t operation)
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

static void audioOperation(spiAudioOp_t operation)
{
    switch (operation)
    {
        //TODO add sweeping functionality and random (noise) mode
        case RESET_AUOP:
        {
            Audio_reset();
            break;
        }
        case FREQ_SET0_AUOP:
        {
            while (SPIIO_empty());//Wait for data
            Audio_setFreq0(SPIIO_pop());
            break;
        }
        case FREQ_SET1_AUOP:
        {
            while (SPIIO_empty());//Wait for data
            Audio_setFreq1(SPIIO_pop());
            break;
        }
        case FREQ_SET2_AUOP:
        {
            while (SPIIO_empty());//Wait for data
            Audio_setFreq2(SPIIO_pop());
            break;
        }
    }
}
