#ifndef SPIBUS_H
#define SPIBUS_H

#include "bluepill.h"

//Init functions
void SPIBus_init_video();//For video MCU
void SPIBus_init_CPU();//For cpu MCU

void SPIBus_transfer_CPU(uint16_t data);//Blocking, just for testing
uint16_t SPIBus_recieve_video();//Blocking, just for testing

#endif
