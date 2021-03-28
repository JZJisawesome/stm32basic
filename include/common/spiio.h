//Will replace spibus and fifo
#ifndef SPIIO_H
#define SPIIO_H

#define SPIIO_BUFFER_SIZE 128

#include "bluepill.h"

//Common functions

//Functions for video
void SPIIO_video_init();
//bool SPIIO_video_full();//If out buffer is full
//void SPIIO_video_push(uint16_t data);//Only call this if SPIIO_video_full() is false
//void SPIIO_video_flush();//TODO
bool SPIIO_video_empty();//If in buffer is empty
uint16_t SPIIO_video_pop();//Only call this if SPIIO_video_empty() is false

//Functions for cpu
void SPIIO_cpu_init();
bool SPIIO_cpu_full();//If out buffer is full
void SPIIO_cpu_push(uint16_t data);//Only call this if SPIIO_cpu_full() is false
void SPIIO_cpu_flush();
//Flushes only if needed, then waits until buffer has room again
#define SPIIO_cpu_smartBlockingFlush() do {while (SPIIO_cpu_full()) {SPIIO_cpu_flush();}} while (0)
//bool SPIIO_cpu_empty();//If in buffer is empty
//uint16_t SPIIO_cpu_pop();//Only call this if SPIIO_cpu_empty() is false
//void SPIIO_cpu_spiInit();

#endif
