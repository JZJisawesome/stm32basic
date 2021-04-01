#ifndef SPIIO_CPU_H
#define SPIIO_CPU_H

#define SPIIO_CPU_BUFFER_SIZE 128//TODO make this as big as possible

#include "stdint.h"
#include "stdbool.h"

void SPIIO_cpu_init();
bool SPIIO_full();//If out buffer is full
void SPIIO_push(uint16_t data);//Only call this if SPIIO_full() is false (or after SPIIO_smartFlush() or SPIIO_forcedFlush())
void SPIIO_smartFlush();//Flushes only if needed (blocks until SPIIO_full()) but MAY NOT FLUSH also (CAREFUL)
void SPIIO_forcedFlush();//Blocks until a flush stops, then flushes (recommended for flush guarantee)

#endif
