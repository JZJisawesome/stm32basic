//Will replace spibus and fifo
#ifndef SPIIO_H
#define SPIIO_H

//Common functions
void SPIIO_extiInit();//Should be called at the very start of main

//Functions for video
void SPIIO_video_spiInit();

//Functions for cpu
void SPIIO_cpu_spiInit();

#endif
