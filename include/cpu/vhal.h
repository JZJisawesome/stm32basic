#ifndef VHAL_H
#define VHAL_H

#include "bluepill.h"
#include "spiio.h"

//Flushing
#define VHAL_flush() do {SPIIO_flush();} while (0)

//Screen Manipulation
void VHAL_scrollUp();

//Text/Character drawing
void VHAL_drawText_atPos(uint16_t x, uint16_t y, const char* string);
void VHAL_drawText(const char* string);

//Line drawing
void VHAL_drawLine_atPos(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void VHAL_drawLineTo(uint16_t x, uint16_t y);

//Position management
void VHAL_setPos(uint16_t x, uint16_t y);

#endif
