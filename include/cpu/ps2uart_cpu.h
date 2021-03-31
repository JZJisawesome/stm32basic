#ifndef PS2UART_CPU_H
#define PS2UART_CPU_H

#define PS2_BUFFER_SIZE 32

#include "bluepill.h"

void PS2UART_cpu_init();
bool PS2UART_empty();//If buffer is empty
uint8_t PS2UART_pop();//Only call this if PS2_empty() is false

#endif
