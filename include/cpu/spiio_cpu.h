#ifndef SPIIO_CPU_H
#define SPIIO_CPU_H

#define SPIIO_CPU_BUFFER_SIZE 128//TODO make this as big as possible

#include "bluepill.h"

void SPIIO_cpu_init();
bool SPIIO_full();//If out buffer is full
void SPIIO_push(uint16_t data);//Only call this if SPIIO_full() is false (or after SPIIO_smartBlockingFlush())
void SPIIO_flush();
//Flushes only if needed, then waits until buffer has room again
#define SPIIO_smartBlockingFlush() do {while (SPIIO_full()) {SPIIO_flush();}} while (0)

#endif
