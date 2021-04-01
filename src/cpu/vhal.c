#include "vhal.h" 

#include "stdint.h"

//TODO add support for audio played on video mcu

#include "bluepill.h"
#include "spiio_cpu.h"

//Position management
void VHAL_setPos(uint16_t x, uint16_t y)
{
    SPIIO_smartFlush();
    SPIIO_push((3 << 9) | x);//Set x position
    SPIIO_smartFlush();
    SPIIO_push((4 << 9) | y);//Set y position
}

//Screen Manipulation
void VHAL_clear()
{
    SPIIO_smartFlush();
    SPIIO_push((1 << 9) | 0);//Screen manipulation command 0: clear
}

void VHAL_fill()
{
    SPIIO_smartFlush();
    SPIIO_push((1 << 9) | 1);//Screen manipulation command 1: fill
}

void VHAL_scrollUp()
{
    SPIIO_smartFlush();
    SPIIO_push((1 << 9) | 2);//Screen manipulation command 2: scroll up
}

void VHAL_scrollDown()
{
    SPIIO_smartFlush();
    SPIIO_push((1 << 9) | 3);//Screen manipulation command 3: scroll down
}

//Text/Character drawing
void VHAL_drawChar_atPos(uint16_t x, uint16_t y, char character)
{
    VHAL_setPos(x, y);
    VHAL_drawChar(character);
}

void VHAL_drawChar(char character)
{
    SPIIO_smartFlush();
    SPIIO_push(character);//Pushing characters is designed to be fast
}

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
        SPIIO_smartFlush();
        SPIIO_push((2 << 9) | character);//First character is sent along with string write command
        
        while (true)
        {
            uint16_t command;
            ++string;
            character = *string;
            
            command = character;
            
            if (!character)
            {
                SPIIO_smartFlush();
                SPIIO_push(command);
                break;
            }
            
            ++string;
            character = *string;
            
            command |= character << 8;
            
            SPIIO_smartFlush();
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
    SPIIO_smartFlush();
    SPIIO_push((5 << 9) | x);//Start multi command transfer of line coords and send x
    SPIIO_smartFlush();
    SPIIO_push(y);//End with sending y coordinate
}

//Shape drawing
void VHAL_drawRectangle_atPos(uint32_t x, uint32_t y, uint32_t xCount, uint32_t yCount)
{
    VHAL_setPos(x, y);
    VHAL_drawRectangle(xCount, yCount);
}

void VHAL_drawRectangle(uint32_t xCount, uint32_t yCount)
{
    SPIIO_smartFlush();
    SPIIO_push((8 << 9) | xCount);//Start multi command transfer of rect size and send x size
    SPIIO_smartFlush();
    SPIIO_push(yCount);//End with sending y size
}

void VHAL_drawTriangle_atPos(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    VHAL_setPos(x0, y0);
    VHAL_drawTriangle(x1, y1, x2, y2);
}

void VHAL_drawTriangle(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    SPIIO_smartFlush();
    SPIIO_push((9 << 9) | x1);//Start multi command transfer of tri and send x1
    SPIIO_smartFlush();
    SPIIO_push(y1);
    SPIIO_smartFlush();
    SPIIO_push(x2);
    SPIIO_smartFlush();
    SPIIO_push(y2);
}

void VHAL_drawCircle_atPos(uint32_t x, uint32_t y, uint32_t radius)
{
    VHAL_setPos(x, y);
    VHAL_drawCircle(radius);
}

void VHAL_drawCircle(uint32_t radius)
{
    SPIIO_smartFlush();
    SPIIO_push((10 << 9) | radius);
}
