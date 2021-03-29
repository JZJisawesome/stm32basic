//PB3 is clock, PB4 is data
#ifndef PS2_H
#define PS2_H

#define PS2_BUFFER_SIZE 128

#include "bluepill.h"

void PS2_init();
bool PS2_empty();//If buffer is empty
uint8_t PS2_pop();//Only call this if PS2_empty() is false

#endif
