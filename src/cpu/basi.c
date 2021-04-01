#include "basi.h" 

#include "bluepill.h"
#include "ps2uart_cpu.h"
#include "vhal.h"

//Thanks https://stackoverflow.com/questions/40591312/c-macro-how-to-get-an-integer-value-into-a-string-literal/40591483
#define STR_IMPL_(x) #x      //stringify argument
#define STR(x) STR_IMPL_(x)  //indirection to expand argument macros

//https://stackoverflow.com/questions/12991054/can-we-use-doubly-linked-list-in-c-without-dynamic-memory-allocation
//http://www.c64os.com/post/basicmemorymanagement <- INCREDIBLY USEFUL
//https://www.c64-wiki.com/wiki/Array
//https://www.c64-wiki.com/wiki/Variable

//Lines are tokenized from keyboard input and stored into program memory
typedef struct line_t line_t;
struct line_t
{
    line_t* nextLine;//If null, there is no next line
    uint16_t lineNumber;
    uint8_t tokens[];
};

//Variables are stored into variable memory (are 6 byte each)
typedef struct
{
    //Identifiers (name and type) (2 bytes total)
    char nameLetter1 : 7;//Only need 7 bits for ascii
    char nameLetter2 : 7;
    enum {REAL, INTEGER, STRING} variableType : 2;
    
    //Data (4 bytes)
    union
    {
        //Scalar types
        float real;
        uint32_t integer;
        char* string;//Points to string (in program or string memory)//NOTE can point to string in program memory if assigned constant in order to save dynamic memory
        
        //Array types
        //If a variable is accessed like an array from BASIC, transparently use these instead of the above (even though)
        //Pointers to array memory
        float* realArray;
        uint32_t* integerArray;
        char** stringArray;//Points to array of string pointers//NOTE string can point to program memory if assigned constant in order to save dynamic memory
    };
} variable_t;

static char lineBuffer[BASIC_LINE_LENGTH];
static uint8_t basicMem[BASIC_BYTES];
static const line_t* programMemoryPointer = (line_t*)(basicMem);//Where program memory starts after run command
static variable_t* variableMemoryStartPointer;//Should be set to byte after program memory after run command
static variable_t* variableMemoryEndPointer;//Should be set to byte after program memory after run command (will grow to hold variables)
//static type?* arrayMemoryStartPointer;//TODO figure out
//static type?* arrayMemoryEndPointer;//TODO figure out
//static type?* stringMemoryStartPointer;//TODO figure out
//static type?* stringMemoryEndPointer;//TODO figure out

static void interpretLine();//The heavy lifting function

void BASIC_init()//Init data structures
{
    VHAL_drawText(STR(BASIC_BYTES)" basic bytes free :)\n");
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
                        if (lineBufferPointer < (BASIC_LINE_LENGTH - 1))//Leave 1 byte for enter key
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
    //NOTE: only interpret lineBuffer up to point where null byte is encountered
    
    VHAL_drawText(lineBuffer);//TESTING
    VHAL_drawChar('\n');//TESTING
    
    //TODO tokenize, then
}
