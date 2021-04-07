#include "ps2uart_video.h"

#include <stdint.h>
#include <stdbool.h>

#include "bluepill.h"
#include "softrenderer.h"//TESTING
#include "communication_defs.h"

//FIXME caps lock shouldn't apply to numbers

static enum {START = -1, PARITY = 8, STOP = 9} state = START;//States 0 to 7 are bits 0 to 7
static uint8_t byteBuffer;

static bool leftShifted = false;
static bool rightShifted = false;
static bool leftControl = false;
static bool rightControl = false;
static bool leftAlt = false;
static bool rightAlt = false;
static bool capsLock = false;
static bool numLock = false;
static bool scrollLock = false;
static bool eRecieved = false;//0xE0 or 0xE1
static bool f0Recieved = false;//0xF0

static void handleByte();
static void processSendable();
static void sendData(uint8_t data);
static char toAscii();

void PS2UART_video_init()
{
    //Set PB4 and PB7 high, then as open-collector outputs
    GPIOB_BSRR = 0b0000000010010000;
    GPIOB_CRL |= 0x70070000;
    //Set PA2 as AF push-pull output
    GPIOA_CRL = (GPIOA_CRL & 0xFFFFF0FF) | 0x00000B00;
    
    //Setup uart
    USART2_BRR = 0x0010;//Set baud rate to 2.25mbit
    USART2_CR1 = 0b0010000000001000;//Enable uart (transmitter only, 8 data bits)
    
    //Initialize EXTI4 for PB4 for negative edges
    AFIO_EXTICR2 |= 0x00000001;//Map PB4 as exti 4
    EXTI_FTSR |= (1 << 4);//Set exti 4 to negative edge
    EXTI_IMR |= (1 << 4);//Disable interrupt mask for exti 4
    NVIC_IPR2 |= 0x00100000;//Set exti 4 priority to one below composite TIM4 (ps/2 is slow so it's not too big of a deal)
    NVIC_ISER0 = (1 << 10);//Enable exti 4 in the nvic
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
        switch (state)
        {
            case START:
            {
                byteBuffer = 0;//So that we can or in bits
                break;
            }
            case PARITY:
            {
                //TODO check parity first, and only handle byte if its correct
                handleByte();
                
            }//Fallthrough to break
            case STOP:
            {
                break;
            }
            default://Storing a bit
            {
                bool bit = (portBPins >> 7) & 1;//Move PB7 to first bit position
                byteBuffer |= bit << state;//Or-in bit into proper position
                
                break;
            }
        }
        
        if (state == STOP)
            state = START;
        else
            ++state;
    }
}

static void handleByte()
{
    if (f0Recieved)//A key was released
    {
        switch (byteBuffer)
        {
            case 0x12:
            {
                leftShifted = false;
                break;
            }
            case 0x59:
            {
                rightShifted = false;
                break;
            }
            case 0x14:
            {
                if (eRecieved)
                {
                    rightControl = false;
                    eRecieved = false;//The 0xE0 was dealt with
                }
                else
                    leftControl = false;
                
                break;
            }
            case 0x11:
            {
                if (eRecieved)
                {
                    rightAlt = false;
                    eRecieved = false;//The 0xE0 was dealt with
                }
                else
                    leftAlt = false;
                
                break;
            }
            default:
            {
                break;//Don't do anything when a key is released
            }
        }
        
        f0Recieved = false;//The 0xF0 was dealt with
    }
    else//Key pressed
    {
        switch (byteBuffer)
        {
            case 0x58:
            {
                capsLock = !capsLock;//Toggle caps lock
                break;
            }
            case 0x12:
            {
                leftShifted = true;
                break;
            }
            case 0x59:
            {
                rightShifted = true;
                break;
            }
            case 0x14:
            {
                if (eRecieved)
                {
                    rightControl = true;
                    eRecieved = false;//The 0xE0 was dealt with
                }
                else
                    leftControl = true;
                
                break;
            }
            case 0x11:
            {
                if (eRecieved)
                {
                    rightAlt = true;
                    eRecieved = false;//The 0xE0 was dealt with
                }
                else
                    leftAlt = true;
                
                break;
            }
            case 0x77:
            {
                numLock = !numLock;
                break;
            }
            case 0x7E:
            {
                scrollLock = !scrollLock;
                break;
            }
            case 0xF0:
            {
                f0Recieved = true;
                break;
            }
            case 0xE0:
            case 0xE1:
            {
                eRecieved = true;
                break;
            }
            default:
            {
                //The byte sent does not modify internal state (ex. shift) but instead
                //provides some sort of information to the cpu (ex. ascii data, arrow key press, etc)
                //Decide the data to send and send it over uart
                processSendable();
                
                if (eRecieved)
                    eRecieved = false;//The e0 or e1 would have been handled by processSendable
                break;
            }
        }
    }
}

static void processSendable()
{
    //TODO other special keys (arrows, home, end, page up/down, fn keys, print screen, pause break)
    //For keys that do not directly translate to ascii, don't use bottom special characters, send byte with high bit set instead of cleared
    
    const bool shifted = leftShifted || rightShifted;//Used by number row
    const bool capalized = shifted ^ capsLock;//Used by letters
    
    uint8_t dataToSend;
    
    switch (byteBuffer)
    {
        case 0x70:
        {
            if (eRecieved)//Insert was pushed
                dataToSend = PS2_INSERT;
            else //Keypad 0 if numlock on, keypad delete if numlock off
                dataToSend = numLock ? '0' : PS2_INSERT;
            
            break;
        }
        case 0x71:
        {
            if (eRecieved)//Delete was pushed
                dataToSend = 0x7F;
            else//Keypad period if numlock on, keypad delete if numlock off
                dataToSend = numLock ? '.' : 0x7F;
            
            break;
        }
        case 0x4A:
        {
            if (eRecieved)//Keypad / was pushed
                dataToSend = '/';
            else//Regular ? and / key was pushed
                dataToSend = shifted ? '?' : '/';
            break;
        }
        case 0x5A:
        {
            dataToSend = '\n';//Keypad and regular enter
            break;
        }
        case 0x76:
        {
            dataToSend = 0x1B;//Escape
            break;
        }
        case 0x66:
        {
            dataToSend = 0x08;//Backspace
            break;
        }
        case 0x0D:
        {
            dataToSend = '\t';
            break;
        }
        case 0x0E:
        {
            dataToSend = shifted ? '~' : '`';
            break;
        }
        case 0x16: 
        {
            dataToSend = shifted ? '!' : '1';
            break;
        }
        case 0x1E:
        {
            dataToSend = shifted ? '@' : '2';
            break;
        }
        case 0x26:
        {
            dataToSend = shifted ? '#' : '3';
            break;
        }
        case 0x25:
        {
            dataToSend = shifted ? '$' : '4';
            break;
        }
        case 0x2E:
        {
            dataToSend = shifted ? '%' : '5';
            break;
        }
        case 0x36:
        {
            dataToSend = shifted ? '^' : '6';
            break;
        }
        case 0x3D:
        {
            dataToSend = shifted ? '&' : '7';
            break;
        }
        case 0x3E:
        {
            dataToSend = shifted ? '*' : '8';
            break;
        }
        case 0x46:
        {
            dataToSend = shifted ? '(' : '9';
            break;
        }
        case 0x45:
        {
            dataToSend = shifted ? ')' : '0';
            break;
        }
        case 0x4E:
        {
            dataToSend = shifted ? '_' : '-';
            break;
        }
        case 0x55:
        {
            dataToSend = shifted ? '+' : '=';
            break;
        }
        case 0x15:
        {
            dataToSend = capalized ? 'Q' : 'q';
            break;
        }
        case 0x1D:
        {
            dataToSend = capalized ? 'W' : 'w';
            break;
        }
        case 0x24:
        {
            dataToSend = capalized ? 'E' : 'e';
            break;
        }
        case 0x2D:
        {
            dataToSend = capalized ? 'R' : 'r';
            break;
        }
        case 0x2C:
        {
            dataToSend = capalized ? 'T' : 't';
            break;
        }
        case 0x35:
        {
            dataToSend = capalized ? 'Y' : 'y';
            break;
        }
        case 0x3C:
        {
            dataToSend = capalized ? 'U' : 'u';
            break;
        }
        case 0x43:
        {
            dataToSend = capalized ? 'I' : 'i';
            break;
        }
        case 0x44:
        {
            dataToSend = capalized ? 'O' : 'o';
            break;
        }
        case 0x4D:
        {
            dataToSend = capalized ? 'P' : 'p';
            break;
        }
        case 0x54:
        {
            dataToSend = shifted ? '{' : '[';
            break;
        }
        case 0x5B:
        {
            dataToSend = shifted ? '}' : ']';
            break;
        }
        case 0x1C:
        {
            dataToSend = capalized ? 'A' : 'a';
            break;
        }
        case 0x1B:
        {
            dataToSend = capalized ? 'S' : 's';
            break;
        }
        case 0x23:
        {
            dataToSend = capalized ? 'D' : 'd';
            break;
        }
        case 0x2B:
        {
            dataToSend = capalized ? 'F' : 'f';
            break;
        }
        case 0x34:
        {
            dataToSend = capalized ? 'G' : 'g';
            break;
        }
        case 0x33:
        {
            dataToSend = capalized ? 'H' : 'h';
            break;
        }
        case 0x3B:
        {
            dataToSend = capalized ? 'J' : 'j';
            break;
        }
        case 0x42:
        {
            dataToSend = capalized ? 'K' : 'k';
            break;
        }
        case 0x4B:
        {
            dataToSend = capalized ? 'L' : 'l';
            break;
        }
        case 0x4C:
        {
            dataToSend = shifted ? ':' : ';';
            break;
        }
        case 0x52:
        {
            dataToSend = shifted ? '"' : '\'';
            break;
        }
        case 0x1A:
        {
            dataToSend = capalized ? 'Z' : 'z';
            break;
        }
        case 0x22:
        {
            dataToSend = capalized ? 'X' : 'x';
            break;
        }
        case 0x21:
        {
            dataToSend = capalized ? 'C' : 'c';
            break;
        }
        case 0x2A:
        {
            dataToSend = capalized ? 'V' : 'v';
            break;
        }
        case 0x32:
        {
            dataToSend = capalized ? 'B' : 'b';
            break;
        }
        case 0x31:
        {
            dataToSend = capalized ? 'N' : 'n';
            break;
        }
        case 0x3A:
        {
            dataToSend = capalized ? 'M' : 'm';
            break;
        }
        case 0x41:
        {
            dataToSend = shifted ? '<' : ',';
            break;
        }
        case 0x49:
        {
            dataToSend = shifted ? '>' : '.';
            break;
        }
        case 0x29:
        {
            dataToSend = ' ';
            break;
        }
        case 0x5D:
        {
            dataToSend = shifted ? '|' : '\\';
            break;
        }
        case 0x69:
        {
            dataToSend = numLock ? '1' : PS2_END;
            break;
        }
        case 0x72:
        {
            dataToSend = numLock ? '2' : PS2_DOWN;
            break;
        }
        case 0x7A:
        {
            dataToSend = numLock ? '3' : PS2_PGDOWN;
            break;
        }
        case 0x6B:
        {
            dataToSend = numLock ? '4' : PS2_LEFT;
            break;
        }
        case 0x73:
        {
            if (numLock)
                dataToSend = '5';
            else
                return;//We don't actually send anything
            break;
        }
        case 0x74:
        {
            dataToSend = numLock ? '6' : PS2_RIGHT;
            break;
        }
        case 0x6C:
        {
            dataToSend = numLock ? '7' : PS2_HOME;
            break;
        }
        case 0x75:
        {
            dataToSend = numLock ? '8' : PS2_UP;
            break;
        }
        case 0x7D:
        {
            dataToSend = numLock ? '9' : PS2_PGUP;
            break;
        }
        case 0x7C:
        {
            dataToSend = '*';
            break;
        }
        case 0x7B:
        {
            dataToSend = '-';
            break;
        }
        case 0x79:
        {
            dataToSend = '+';
            break;
        }
        case 0x05:
        {
            dataToSend = PS2_FN1;
            break;
        }
        case 0x06:
        {
            dataToSend = PS2_FN2;
            break;
        }
        case 0x04:
        {
            dataToSend = PS2_FN3;
            break;
        }
        case 0x0C:
        {
            dataToSend = PS2_FN4;
            break;
        }
        case 0x03:
        {
            dataToSend = PS2_FN5;
            break;
        }
        case 0x0B:
        {
            dataToSend = PS2_FN6;
            break;
        }
        case 0x83:
        {
            dataToSend = PS2_FN7;
            break;
        }
        case 0x0A:
        {
            dataToSend = PS2_FN8;
            break;
        }
        case 0x01:
        {
            dataToSend = PS2_FN9;
            break;
        }
        case 0x09:
        {
            dataToSend = PS2_FN10;
            break;
        }
        case 0x78:
        {
            dataToSend = PS2_FN11;
            break;
        }
        case 0x07:
        {
            dataToSend = PS2_FN12;
            break;
        }
    }
    
    //Wait for transmit buffer to empty (should never happen because
    //ps/2 is much slower than uart, even with a slow baud rate)
    //while (!(USART2_SR & (1 << 7)));//NOTE: Commented out because unneeded
    USART2_DR = dataToSend;//Write data
}
