#ifndef AUDIO_H
#define AUDIO_H

//Run timer at 4.5MHz
//Feels like a good compromise; minimum of 70hz, good resolution under about 2000hz.
//Also a factor of 72MHz, which makes for a nice prescaler value (16)
//Note:
//Frequency doesn't affect pitch of music; just reciprocal curve of reload value vs freq
//Higher=better resolution (less aggressive curve), but higher lowest possible frequency
//Lower=worse resolution (more agressive curve), but lower possible lowest frequency
//y=TIMER_FREQ/value of ARR between 0 and 65535
#define AUDIO_TIMER_FREQ 4500000

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
