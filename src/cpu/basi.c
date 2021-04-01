#include "basi.h" 

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

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
    uint16_t lineNumber;//Can be between 1 and 0xFFFF
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
        int32_t integer;
        char* string;//Points to string (in program or string memory)//NOTE can point to string in program memory if assigned constant in order to save dynamic memory
        
        //Array types
        //If a variable is accessed like an array from BASIC, transparently use these instead of the above (even though)
        //Pointers to array memory
        float* realArray;
        int32_t* integerArray;
        char** stringArray;//Points to array of string pointers//NOTE string can point to program memory if assigned constant in order to save dynamic memory
    };
} variable_t;

static uint8_t basicMem[BASIC_BYTES];//Regular basic memory

static const line_t* programMemoryPointer = (line_t*)(basicMem);//Where program memory starts after run command
static variable_t* variableMemoryStartPointer;//Should be set to byte after program memory after run command
static variable_t* variableMemoryEndPointer;//Should be set to byte after program memory after run command (will grow to hold variables)
//static type?* arrayMemoryStartPointer;//TODO figure out
//static type?* arrayMemoryEndPointer;//TODO figure out
//static type?* stringMemoryStartPointer;//TODO figure out
//static type?* stringMemoryEndPointer;//TODO figure out

static void interpretLine(const char* enteredText);
static void tokenize(const char* enteredText, line_t* output);

void BASIC_init()//Init data structures
{
    VHAL_drawText(STR(BASIC_BYTES)" basic bytes free :)\n");
}

void BASIC_begin()//Begin basic interpreter
{
    while (true)
    {
        static char lineBuffer[BASIC_LINE_LENGTH];//Buffer for entered text
        
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
                    //TODO handle arrows, delete, insert, etc
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
            else//While we're waiting for more key strokes, flush things that were typed to the screen
                VHAL_flush();
        }
        
        interpretLine(lineBuffer);//Interpret contents of line buffer
    }
}

static void interpretLine(const char* enteredText)
{
    //NOTE: only interpret enteredText up to point where null byte is encountered
    
    VHAL_drawText(enteredText);//TESTING
    VHAL_drawChar('\n');//TESTING
    
    //Tokenize enteredText
    union
    {
        line_t tokenizedLine;
        char storage[BASIC_LINE_LENGTH + 16];//A little bit of headroom in case entire line is filled and tokenize needs extra room
    } buffer;
    tokenize(enteredText, &buffer.tokenizedLine);
    
    VHAL_drawChar(buffer.tokenizedLine.lineNumber);//TESTING
    VHAL_drawChar('\n');//TESTING
    
    //TODO now interpret (either execute or put into program memory)
    //If 0, should be executed immediatly, else put into program memory in correct place
}

static void tokenize(const char* enteredText, line_t* output)//Tokenize enteredText and return in output (NOTE: leaves nextLine unchanged)
{
    output->lineNumber = (uint16_t)(strtoul(enteredText, NULL, 10));//Extract line number (if it exists)
    while (isdigit((int)(*enteredText))) {++enteredText;}//Skip to the first non-number character
    //TODO tokenize rest of line
}
