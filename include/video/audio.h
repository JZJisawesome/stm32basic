#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

//TODO implement
//NOTE: If the buffer runs out, the audio will stop playing (but not Audio_stop; it will automatically resume as more is pushed)

void Audio_init();
void Audio_reset();
void Audio_setFreq0(uint_fast16_t frequency);
void Audio_setFreq1(uint_fast16_t frequency);
void Audio_setFreq2(uint_fast16_t frequency);
//TODO add sweeping functionality and random (noise) mode (maybe optional timeout and "enevelope"; see nes apu for ideas)
//Also maybe different waveforms?

#endif
