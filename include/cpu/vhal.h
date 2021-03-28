#ifndef VHAL_H
#define VHAL_H

#include "bluepill.h"
#include "spiio.h"

//Flushing (can save time if a draw call would otherwise stall)
#define VHAL_flush() do {SPIIO_flush();} while (0)

//Position management
void VHAL_setPos(uint16_t x, uint16_t y);

//Screen Manipulation
void VHAL_clear();
void VHAL_fill();
void VHAL_scrollUp();
void VHAL_scrollDown();

//Text/Character drawing
void VHAL_drawChar_atPos(uint16_t x, uint16_t y, char character);
void VHAL_drawChar(char character);
void VHAL_drawText_atPos(uint16_t x, uint16_t y, const char* string);
void VHAL_drawText(const char* string);

//Line drawing
void VHAL_drawLine_atPos(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void VHAL_drawLineTo(uint16_t x, uint16_t y);
//TODO horizontal and vertical line support

//Shape drawing
void VHAL_drawRectangle_atPos(uint32_t x, uint32_t y, uint32_t xCount, uint32_t yCount);
void VHAL_drawRectangle(uint32_t xCount, uint32_t yCount);

#endif
