#include "basi.h" 

#include "bluepill.h"
#include "ps2uart_cpu.h"
#include "vhal.h"

#define LINE_LENGTH 128

//Lines are 128 characters long


//https://stackoverflow.com/questions/12991054/can-we-use-doubly-linked-list-in-c-without-dynamic-memory-allocation <- Useful

//http://www.c64os.com/post/basicmemorymanagement <- INCREDIBLY USEFUL********************************************************************

/*
typedef struct
{
    
} lineEntry_t;*/

static char lineBuffer[LINE_LENGTH];

static void interpretLine();//The heavy lifting function

void BASIC_init()//Init data structures
{
    VHAL_drawText("????? basic bytes free :)\n");
}

void BASIC_begin()//Begin basic interpreter
{
    while (true)
    {
        VHAL_drawChar('>');
        VHAL_flush();//Force cursor to show up
        
        bool enterPressed = false;
        uint_fast8_t lineBufferPointer = 0;
        while (!enterPressed)
        {
            if (!PS2UART_empty())//Wait for key to be pressed
            {
                uint8_t key = PS2UART_pop();
                switch (key)
                {
                    case 0x08://Backspace
                    {
                        if (lineBufferPointer != 0)
                        {
                            VHAL_drawChar(0x08);
                            --lineBufferPointer;
                        }
                        
                        break;
                    }
                    case '\n':
                    {
                        enterPressed = true;
                        VHAL_drawChar('\n');
                        lineBuffer[lineBufferPointer] = 0x00;//End line with null byte
                        //No need to increment line pointer because it's the end of the line
                        //and it will be reset next time
                        break;
                    }
                    default:
                    {
                        //A character was typed
                        if (lineBufferPointer < (LINE_LENGTH - 1))//Leave 1 byte for enter key
                        {
                            VHAL_drawChar(key);
                            lineBuffer[lineBufferPointer] = key;
                            ++lineBufferPointer;
                        }
                        break;
                    }
                }
            }
            else//While we're waiting for key strokes, flush things that were typed to the screen
                VHAL_flush();
        }
        
        interpretLine();//Interpret contents of line buffer
    }
}

static void interpretLine()
{
    //NOTE: only interpret up to point where null byte is encountered
    
    VHAL_drawText(lineBuffer);//TESTING
    VHAL_drawChar('\n');//TESTING
}
