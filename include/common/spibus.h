#ifndef SPIBUS_H
#define SPIBUS_H

#include "bluepill.h"

//Init functions
void SPIBus_init_video();//For video MCU
void SPIBus_init_CPU();//For cpu MCU

void SPIBus_transfer_CPU(uint16_t data);//Blocking, just for testing
#define SPIBus_recieve_video() (SPI2_DR)

void SPIBus_disableInterrupts_video();
void SPIBus_enableInterrupts_video();

#endif
