#ifndef FIFO_H
#define FIFO_H

#define FIFO_SIZE 128

#include "bluepill.h"

typedef struct
{
    uint16_t data[FIFO_SIZE];
    uint16_t readPointer;//Next place to read
    uint16_t writePointer;//Next place to write to
} fifo_t;

void FIFO_init(fifo_t* fifo);

bool FIFO_isFull(fifo_t* fifo);
bool FIFO_isEmpty(fifo_t* fifo);

//NOTE: These are not safe: check FIFO_isFull and FIFO_isEmpty before using these
void FIFO_push(fifo_t* fifo, uint16_t data);
uint16_t FIFO_pop(fifo_t* fifo);

#endif
