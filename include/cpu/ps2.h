//PB4 is clock, PB7 is data
#ifndef PS2_H
#define PS2_H

#define PS2_BUFFER_SIZE 128
#define KEYBOARD_SET 2

#include "bluepill.h"

void PS2_init();
bool PS2_empty();//If buffer is empty
uint8_t PS2_pop();//Only call this if PS2_empty() is false
char PS2_toAscii(uint16_t keyboardData, bool capital);//NOTE: keyboardData should still be inspected for special codes

#endif
