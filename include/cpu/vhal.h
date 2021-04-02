#ifndef VHAL_H
#define VHAL_H

//NOTE: VHAL functions only block if the spiio buffer runs out of room
//(they all always call SPIIO_smartFlush() before any pushes internally)

//TODO add support for audio played on video mcu

#include <stdint.h>

#include "spiio_cpu.h"

//Flushing (can save time if a draw call would otherwise stall)
#define VHAL_flush() do {SPIIO_forcedFlush();} while (0)

//Position management
void VHAL_setCharPos(uint_fast8_t x, uint_fast8_t y);//Character, not pixel, coordinates
//void VHAL_setPos(uint16_t x, uint16_t y);

//Screen Manipulation
void VHAL_clear();
void VHAL_fill();
void VHAL_scrollUp();
void VHAL_scrollDown();

//Text/Character drawing
void VHAL_drawChar_atPos(uint_fast8_t x, uint_fast8_t y, char character);//Character, not pixel, coordinates
void VHAL_drawChar(char character);
void VHAL_drawText_atPos(uint_fast8_t x, uint_fast8_t y, const char* string);//Character, not pixel, coordinates
void VHAL_drawText(const char* string);

//Line drawing
void VHAL_drawLine(uint_fast16_t x0, uint_fast16_t y0, uint_fast16_t x1, uint_fast16_t y1);
//TODO horizontal and vertical line support

//void VHAL_drawLine_atPos(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
//void VHAL_drawLineTo(uint16_t x, uint16_t y);

//Shape drawing
void VHAL_drawRectangle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t xCount, uint_fast16_t yCount);//TODO
void VHAL_drawTriangle(uint_fast16_t x0, uint_fast16_t y0, uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2);//TODO
void VHAL_drawCircle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t radius);//TODO

//void VHAL_drawRectangle_atPos(uint32_t x, uint32_t y, uint32_t xCount, uint32_t yCount);
//void VHAL_drawRectangle(uint32_t xCount, uint32_t yCount);
//void VHAL_drawTriangle_atPos(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
//void VHAL_drawTriangle(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
//void VHAL_drawCircle_atPos(uint32_t x, uint32_t y, uint32_t radius);
//void VHAL_drawCircle(uint32_t radius);

#endif
