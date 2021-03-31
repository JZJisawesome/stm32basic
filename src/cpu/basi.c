#include "basi.h" 

#include "bluepill.h"
#include "ps2uart_cpu.h"
#include "vhal.h"

//TODO create PS/2 keyboard code ps2.c and ps2.h with buffer; filled by interrupts on edges of clock sent by keyboard
//Then this code can poll isEmpty and wait for it to be false, then interpret the key that was pressed

//https://stackoverflow.com/questions/12991054/can-we-use-doubly-linked-list-in-c-without-dynamic-memory-allocation <- Useful

//http://www.c64os.com/post/basicmemorymanagement <- INCREDIBLY USEFUL********************************************************************

/*
typedef struct
{
    
} lineEntry_t;*/

void BASIC_init()//Init data structures
{
    //TODO maybe move keyboard code/ascii translation to video mcu and send over serial?
}

void BASIC_begin()//Begin basic interpreter
{
    //TODO maybe move keyboard code/ascii translation to video mcu and send over serial?
}
