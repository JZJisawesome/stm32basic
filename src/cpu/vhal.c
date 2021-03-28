#include "vhal.h" 

#include "bluepill.h"
#include "spiio_cpu.h"

//Screen Manipulation
void VHAL_clear()
{
    SPIIO_smartBlockingFlush();
    SPIIO_push((1 << 9) | 0);//Screen manipulation command 0: clear
}

void VHAL_fill()
{
    SPIIO_smartBlockingFlush();
    SPIIO_push((1 << 9) | 1);//Screen manipulation command 1: fill
}

void VHAL_scrollUp()
{
    SPIIO_smartBlockingFlush();
    SPIIO_push((1 << 9) | 2);//Screen manipulation command 2: scroll up
}

void VHAL_scrollDown()
{
    SPIIO_smartBlockingFlush();
    SPIIO_push((1 << 9) | 3);//Screen manipulation command 3: scroll down
}

//Text/Character drawing
void VHAL_drawText_atPos(uint16_t x, uint16_t y, const char* string)
{
    VHAL_setPos(x, y);
    VHAL_drawText(string);
}

void VHAL_drawText(const char* string)
{
    char character = *string;
    if (character)//String does not start with a null byte
    {
        SPIIO_smartBlockingFlush();
        SPIIO_push((2 << 9) | character);//First character is sent along with string write command
        
        while (true)
        {
            uint16_t command;
            ++string;
            character = *string;
            
            command = character;
            
            if (!character)
            {
                SPIIO_smartBlockingFlush();
                SPIIO_push(command);
                break;
            }
            
            ++string;
            character = *string;
            
            command |= character << 8;
            
            SPIIO_smartBlockingFlush();
            SPIIO_push(command);
            
            if (!character)//We do this here because we still have to send the null byte to 
                break;//let the video mcu know the string has ended
        }
    }
}

//Line drawing
void VHAL_drawLine_atPos(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    VHAL_setPos(x0, y0);
    VHAL_drawLineTo(x1, y1);
}

void VHAL_drawLineTo(uint16_t x, uint16_t y)
{
    SPIIO_smartBlockingFlush();
    SPIIO_push((5 << 9) | x);//Start multi command transfer of line coords and send x
    SPIIO_smartBlockingFlush();
    SPIIO_push(y);//End with sending y coordinate
}

//Position management
void VHAL_setPos(uint16_t x, uint16_t y)
{
    SPIIO_smartBlockingFlush();
    SPIIO_push((3 << 9) | x);//Set x position
    SPIIO_smartBlockingFlush();
    SPIIO_push((4 << 9) | y);//Set y position
}
