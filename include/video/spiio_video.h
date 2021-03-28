#ifndef SPIIO_VIDEO_H
#define SPIIO_VIDEO_H

#define SPIIO_VIDEO_BUFFER_SIZE 2048//TODO make this as big as possible

#include "bluepill.h"

void SPIIO_video_init();
bool SPIIO_empty();//If buffer is empty
uint16_t SPIIO_pop();//Only call this if SPIIO_empty() is false

#endif
