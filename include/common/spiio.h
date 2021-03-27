//Will replace spibus and fifo
#ifndef SPIIO_H
#define SPIIO_H

#define SPIIO_BUFFER_SIZE 128

#include "bluepill.h"

//Common functions

//Functions for video
void SPIIO_video_init();
bool SPIIO_video_empty();//If in buffer is empty
uint16_t SPIIO_video_pop();//Only call this if SPIIO_video_empty() is false

//Functions for cpu
void SPIIO_cpu_init();
//void SPIIO_cpu_spiInit();

#endif
