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
void VHAL_setCharRelPos(int_fast8_t offsetX, int_fast8_t offsetY);//Character, not pixel, coordinates
void VHAL_moveCharPosLeft();
void VHAL_moveCharPosRight();
void VHAL_moveCharPosUp();
void VHAL_moveCharPosDown();

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
void VHAL_drawHLine(uint_fast16_t x, uint_fast16_t y, uint_fast16_t length);
void VHAL_drawVLine(uint_fast16_t x, uint_fast16_t y, uint_fast16_t length);

//Shape drawing
void VHAL_drawRectangle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t xCount, uint_fast16_t yCount);//TODO
void VHAL_drawTriangle(uint_fast16_t x0, uint_fast16_t y0, uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2);
void VHAL_drawCircle(uint_fast16_t x, uint_fast16_t y, uint_fast16_t radius);

#endif
