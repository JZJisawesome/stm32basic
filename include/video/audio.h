#ifndef AUDIO_H
#define AUDIO_H

#define AUDIO_BUFFER_SIZE 128//Per channel

#include <stdint.h>
#include <stdbool.h>

//TODO implement
//NOTE: If the buffer runs out, the audio will stop playing (but not Audio_stop; it will automatically resume as more is pushed)

void Audio_init();
void Audio_start0();
void Audio_start1();
void Audio_startBoth();
void Audio_stop0();
void Audio_stop1();
void Audio_stopBoth();
bool Audio_bufferFull0();
bool Audio_bufferFull1();
void Audio_push0(uint_fast16_t duration, uint_fast16_t freq);//Duration is in ms
void Audio_push1(uint_fast16_t duration, uint_fast16_t freq);//Duration is in ms

#endif
