#include "vhal.h" 

#include <stdint.h>
#include <assert.h>

//TODO add support for audio played on video mcu

#include "bluepill.h"
#include "communication_defs.h"
#include "spiio_cpu.h"

//Helper things
static inline void safeCommandPush(spiMajorCommand_t commandMajor, uint_fast16_t minorOrData)
{
    uint_fast16_t command = ((uint16_t)(commandMajor) << 9) | minorOrData;
    
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(command);//Now it's safe to push
}

static inline void screenCommand(spiScreenOp_t screenOp)
{
    safeCommandPush(SCREEN_OP, (uint_fast16_t)(screenOp));
}

//Position management
void VHAL_setCharPos(uint_fast8_t x, uint_fast8_t y)
{
    safeCommandPush(CHAR_POS_SET, x);//Send x coordinate
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y);//Send y coordinate
}

void VHAL_setCharRelPos(int_fast8_t offsetX, int_fast8_t offsetY)
{
    //Cast int_fast8_t to uint_fast16_t, but don't sign extend; just copy lower 8 bits
    //(get rid of sign extended bits by anding with 0xFF)
    uint_fast16_t offsetX16Bit = (uint_fast16_t)(offsetX) & 0xFF;
    uint_fast16_t offsetY16Bit = (uint_fast16_t)(offsetY) & 0xFF;
    
    safeCommandPush(CHAR_RELX_POS_SET, offsetX16Bit);//Send x offset
    safeCommandPush(CHAR_RELY_POS_SET, offsetY16Bit);//Send y offset
}

void VHAL_moveCharPosLeft()
{
    safeCommandPush(CHAR_RELX_POS_SET, (uint_fast16_t)(-1) & 0xFF);//Send x offset
}

void VHAL_moveCharPosRight()
{
    safeCommandPush(CHAR_RELX_POS_SET, 1);//Send x offset
}

void VHAL_moveCharPosUp()
{
    safeCommandPush(CHAR_RELY_POS_SET, (uint_fast16_t)(-1) & 0xFF);//Send y offset
}

void VHAL_moveCharPosDown()
{
    safeCommandPush(CHAR_RELY_POS_SET, 1);//Send y offset
}

//Screen Manipulation
void VHAL_clear()
{
    screenCommand(CLEAR_SCROP);
}

void VHAL_fill()
{
    screenCommand(FILL_SCROP);
}

void VHAL_scrollUp()
{
    screenCommand(SCROLL_UP_SCROP);
}

void VHAL_scrollDown()
{
    screenCommand(SCROLL_DOWN_SCROP);
}

//Text/Character drawing
void VHAL_drawChar_atPos(uint_fast8_t x, uint_fast8_t y, char character)
{
    VHAL_setCharPos(x, y);
    VHAL_drawChar(character);
}

void VHAL_drawChar(char character)
{
    safeCommandPush(CHAR_WRITE, character);
}

void VHAL_drawText_atPos(uint_fast8_t x, uint_fast8_t y, const char* string)
{
    VHAL_setCharPos(x, y);
    VHAL_drawText(string);
}

void VHAL_drawText(const char* string)
{
    char character = *string;
    if (character)//String does not start with a null byte
    {
        safeCommandPush(STRING_WRITE, character);//First character is sent along with string write command
        
        while (true)
        {
            uint_fast16_t command;
            ++string;
            character = *string;
            
            command = character;
            
            if (!character)//Null byte encountered
            {
                SPIIO_smartFlush();//Flush if we are out of room; block until there is
                SPIIO_push(0x00);//Send the null byte
                break;//Stop sending string data
            }
            
            ++string;
            character = *string;
            
            command |= character << 8;
            
            SPIIO_smartFlush();//Flush if we are out of room; block until there is
            SPIIO_push(command);//Send both characters at once
            
            if (!character)//We do this here because we still have to send the null byte to...
                break;//let the video mcu know the string has ended
        }
    }
}

//Line drawing
void VHAL_drawLine(uint_fast16_t x0, uint_fast16_t y0, uint_fast16_t x1, uint_fast16_t y1)
{
    safeCommandPush(LINE_DRAW, x0);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y0);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(x1);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y1);
}

void VHAL_drawHLine(uint_fast16_t x, uint_fast16_t y, uint_fast16_t length)
{
    safeCommandPush(HLINE_DRAW, x);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(length);
}

void VHAL_drawVLine(uint_fast16_t x, uint_fast16_t y, uint_fast16_t length)
{
    safeCommandPush(VLINE_DRAW, x);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(length);
}

//Shape drawing
void VHAL_drawTriangle(uint_fast16_t x0, uint_fast16_t y0, uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2)
{
    safeCommandPush(POLY_DRAW, 3);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(x0);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y0);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(x1);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y1);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(x2);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y2);
}

void VHAL_drawCircle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t radius)
{
    safeCommandPush(CIRCLE_DRAW, x);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(y);
    SPIIO_smartFlush();//Flush if we are out of room; block until there is
    SPIIO_push(radius);
}
/*
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
*/
