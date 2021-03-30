#include "ps2.h"
//TODO maybe move keyboard code/ascii translation to video mcu and send over serial?
//  Only send when pushed, and handle capatilization/multi byte keys and pack into 1 byte
//  No need to use DMA, because a key will be processed and transfered over uart faster than another will arrive
#include "bluepill.h"

static volatile uint8_t buffer[PS2_BUFFER_SIZE];
static uint16_t popPointer = 0;//Next place to pop from
static volatile uint16_t pushPointer = 0;//Next place to push to

void PS2_init()
{
    //Set PB4 and PB7 high, then as open-collector outputs
    GPIOB_BSRR = 0b0000000010010000;
    GPIOB_CRL |= 0x70070000;
    
    //Initialize EXTI3 for PB4 for negative edges
    AFIO_EXTICR2 |= 0x00000001;//Map PB4 as exti 4
    EXTI_FTSR |= (1 << 4);//Set exti 4 to negative edge
    EXTI_IMR |= (1 << 4);//Disable interrupt mask for exti 4
    NVIC_ISER0 = (1 << 10);//Enable exti 4 in the nvic
}

bool PS2_empty()//If buffer is empty
{
    return popPointer == pushPointer;
}

uint8_t PS2_pop()//Only call this if PS2_empty() is false
{
    uint8_t data = buffer[popPointer];
    
    if (popPointer == (PS2_BUFFER_SIZE - 1))
        popPointer = 0;
    else
        ++popPointer;
    
    return data;
}

__attribute__ ((interrupt ("IRQ"))) void __ISR_EXTI4()//Fires every negative edge of the clock
{
    uint32_t portBPins = GPIOB_IDR;//Capture PB7
    EXTI_PR = 1 << 4;//Clear the pending register
    
    //NOTE: For some reason most of my dev boards like to trigger on the posedge and negedge
    //even though EXTI_RTSR is not set. You can test if this is a problem for you by
    //commenting out this if statement with !(portBPins & (1 << 4))
    if (!(portBPins & (1 << 4)))
    {
        static enum {START = -1, PARITY = 8, STOP = 9} state = START;//States 0 to 7 are bits 0 to 7
        
        switch (state)
        {
            case START:
            {
                buffer[pushPointer] = 0;//So that we can or in bits
                break;
            }
            case PARITY:
            {
                //TODO check parity
                break;
            }
            case STOP:
            {
                if (pushPointer == (PS2_BUFFER_SIZE - 1))
                    pushPointer = 0;
                else
                    ++pushPointer;
                
                break;
            }
            default://Storing a bit
            {
                bool bit = (portBPins >> 7) & 1;//Move PB7 to first bit position
                buffer[pushPointer] |= bit << state;//Or-in bit into proper position
                
                break;
            }
        }
        
        if (state == STOP)
            state = START;
        else
            ++state;
    }
}

char PS2_toAscii(uint16_t keyboardData, bool capital)
{//TODO maybe move keyboard code/ascii translation to video mcu and send over serial?
    #if KEYBOARD_SET == 1
        if (capital)
        {
            switch (keyboardData)
            {
                //TODO
                /*
                case 0x0E:
                    return '
                case 0x16: 
                    return '
                case 0x1E:
                    return '
                case 0x26:
                    return '
                case 0x25:
                    return '
                case 0x2E:
                    return '
                case 0x36:
                    return '
                case 0x3D:
                    return '
                case 0x3E:
                    return '
                case 0x46:
                    return '
                case 0x45:
                    return '
                case 0x4E:
                    return '
                case 0x55:
                    return '
                case 0x66:
                    return '
                case 0x0D:
                    return '
                case 0x15:
                    return '
                case 0x1D:
                    return '
                case 0x24:
                    return '
                case 0x2D:
                    return '
                case 0x2C:
                    return '
                case 0x35:
                    return '
                case 0x3C:
                    return '
                case 0x43:
                    return '
                case 0x44:
                    return '
                case 0x4D:
                    return '
                case 0x54:
                    return '
                case 0x5B:
                    return '
                case 0x1C:
                    return '
                case 0x1B:
                    return '
                case 0x23:
                    return '
                case 0x2B:
                    return '
                case 0x34:
                    return '
                case 0x33:
                    return '
                case 0x3B:
                    return '
                case 0x42:
                    return '
                case 0x4B:
                    return '
                case 0x4C:
                    return '
                case 0x52:
                    return '
                case 0x5A:
                    return '
                case 0x1A:
                    return '
                case 0x22:
                    return '
                case 0x21:
                    return '
                case 0x2A:
                    return '
                case 0x32:
                    return '
                case 0x31:
                    return '
                case 0x3A:
                    return '
                case 0x41:
                    return '
                case 0x49:
                    return '
                case 0x4A:
                    return '
                default:
                    return keyboardData;
                */
            }
        }
        else
        {
            switch (keyboardData)
            {
                //TODO
                /*
                */
            }
        }
    #elif KEYBOARD_SET == 2
        //https://techdocs.altium.com/display/FPGA/PS2+Keyboard+Scan+Codes
        if (capital)
        {
            switch (keyboardData)
            {
                case 0x0E:
                    return '~';
                case 0x16: 
                    return '!';
                case 0x1E:
                    return '@';
                case 0x26:
                    return '#';
                case 0x25:
                    return '$';
                case 0x2E:
                    return '%';
                case 0x36:
                    return '^';
                case 0x3D:
                    return '&';
                case 0x3E:
                    return '*';
                case 0x46:
                    return '(';
                case 0x45:
                    return ')';
                case 0x4E:
                    return '_';
                case 0x55:
                    return '+';
                case 0x66:
                    return 0x08;//Backspace
                case 0x0D:
                    return '\t';
                case 0x15:
                    return 'Q';
                case 0x1D:
                    return 'W';
                case 0x24:
                    return 'E';
                case 0x2D:
                    return 'R';
                case 0x2C:
                    return 'T';
                case 0x35:
                    return 'Y';
                case 0x3C:
                    return 'U';
                case 0x43:
                    return 'I';
                case 0x44:
                    return 'O';
                case 0x4D:
                    return 'P';
                case 0x54:
                    return '{';
                case 0x5B:
                    return '}';
                case 0x1C:
                    return 'A';
                case 0x1B:
                    return 'S';
                case 0x23:
                    return 'D';
                case 0x2B:
                    return 'F';
                case 0x34:
                    return 'G';
                case 0x33:
                    return 'H';
                case 0x3B:
                    return 'J';
                case 0x42:
                    return 'K';
                case 0x4B:
                    return 'L';
                case 0x4C:
                    return ':';
                case 0x52:
                    return '"';
                case 0x5A:
                    return '\n';
                case 0x1A:
                    return 'Z';
                case 0x22:
                    return 'X';
                case 0x21:
                    return 'C';
                case 0x2A:
                    return 'V';
                case 0x32:
                    return 'B';
                case 0x31:
                    return 'N';
                case 0x3A:
                    return 'M';
                case 0x41:
                    return '<';
                case 0x49:
                    return '>';
                case 0x4A:
                    return '?';
                case 0x29:
                    return ' ';
                case 0x64:
                    return 0x7F;//Delete
                case 0x6C:
                    return '7';
                case 0x6B:
                    return '4';
                case 0x69:
                    return '1';
                case 0x75:
                    return '8';
                case 0x73:
                    return '5';
                case 0x72:
                    return '2';
                case 0x70:
                    return '0';
                case 0x7C:
                    return '*';
                case 0x7D:
                    return '9';
                case 0x74:
                    return '6';
                case 0x7A:
                    return '3';
                case 0x71:
                    return '.';
                case 0x7B:
                    return '-';
                case 0x79:
                    return '+';
                case 0x5D:
                    return '|';
                default:
                    return keyboardData;
            }
        }
        else//Lowercase
        {
            switch (keyboardData)
            {
                case 0x0E:
                    return '`';
                case 0x16: 
                case 0x69:
                    return '1';
                case 0x1E:
                case 0x72:
                    return '2';
                case 0x26:
                case 0x7A:
                    return '3';
                case 0x25:
                case 0x6B:
                    return '4';
                case 0x2E:
                case 0x73:
                    return '5';
                case 0x36:
                case 0x74:
                    return '6';
                case 0x3D:
                case 0x6C:
                    return '7';
                case 0x3E:
                case 0x75:
                    return '8';
                case 0x46:
                case 0x7D:
                    return '9';
                case 0x45:
                case 0x70:
                    return '0';
                case 0x4E:
                case 0x7B:
                    return '-';
                case 0x55:
                    return '=';
                case 0x66:
                    return 0x08;//Backspace
                case 0x0D:
                    return '\t';
                case 0x15:
                    return 'q';
                case 0x1D:
                    return 'w';
                case 0x24:
                    return 'e';
                case 0x2D:
                    return 'r';
                case 0x2C:
                    return 't';
                case 0x35:
                    return 'y';
                case 0x3C:
                    return 'u';
                case 0x43:
                    return 'i';
                case 0x44:
                    return 'o';
                case 0x4D:
                    return 'p';
                case 0x54:
                    return '[';
                case 0x5B:
                    return ']';
                case 0x1C:
                    return 'a';
                case 0x1B:
                    return 's';
                case 0x23:
                    return 'd';
                case 0x2B:
                    return 'f';
                case 0x34:
                    return 'g';
                case 0x33:
                    return 'h';
                case 0x3B:
                    return 'j';
                case 0x42:
                    return 'k';
                case 0x4B:
                    return 'l';
                case 0x4C:
                    return ';';
                case 0x52:
                    return '\'';
                case 0x5A:
                    return '\n';
                case 0x1A:
                    return 'z';
                case 0x22:
                    return 'x';
                case 0x21:
                    return 'c';
                case 0x2A:
                    return 'v';
                case 0x32:
                    return 'b';
                case 0x31:
                    return 'n';
                case 0x3A:
                    return 'm';
                case 0x41:
                    return ',';
                case 0x49:
                    return '.';
                case 0x4A:
                    return '/';
                case 0x29:
                    return ' ';
                case 0x64:
                    return 0x7F;//Delete
                case 0x7C:
                    return '*';
                case 0x71:
                    return '.';
                case 0x79:
                    return '+';
                case 0x5D:
                    return '\\';
                default:
                    return keyboardData;
            }
        }
    #else//Keyboard set 3
        //TODO (note: lots can be copied from set 2)
    #endif
}
