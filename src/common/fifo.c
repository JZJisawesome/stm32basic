#include "fifo.h"
#include "bluepill.h"

void FIFO_init(fifo_t* fifo)
{
    fifo->readPointer = 0;
    fifo->writePointer = 0;
}

bool FIFO_isFull(fifo_t* fifo)
{
    uint16_t nextWritePointer;
    
    if (fifo->writePointer == (FIFO_SIZE - 1))
        nextWritePointer = 0;
    else
        nextWritePointer = fifo->writePointer + 1;
    
    return nextWritePointer == fifo->readPointer;
}

bool FIFO_isEmpty(fifo_t* fifo)
{
    return fifo->readPointer == fifo->writePointer;
}

void FIFO_push(fifo_t* fifo, uint16_t data)//If full, does nothing
{
    fifo->data[fifo->writePointer] = data;
    
    if (fifo->writePointer == (FIFO_SIZE - 1))
        fifo->writePointer = 0;
    else
        fifo->writePointer += 1;
}

uint16_t FIFO_pop(fifo_t* fifo)//If empty, returns null and does nothing
{
    uint16_t data = fifo->data[fifo->readPointer];
    
    if (fifo->readPointer == (FIFO_SIZE - 1))
        fifo->readPointer = 0;
    else
        fifo->readPointer += 1;
        
    return data;
}
