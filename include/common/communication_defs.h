#ifndef COMMUNICATION_DEFS_H
#define COMMUNICATION_DEFS_H

//SPI Commands
typedef enum
{
    CHAR_WRITE = 0x00, SCREEN_OP = 0x01, STRING_WRITE = 0x02, CHAR_POS_SET = 0x03,
    LINE_DRAW = 0x04, HLINE_DRAW = 0x05, VLINE_DRAW = 0x06, POLY_DRAW = 0x07,
    CIRCLE_DRAW = 0x08, AUDIO_OP = 0x09, CHAR_RELX_POS_SET = 0x0A, CHAR_RELY_POS_SET = 0x0B
} spiMajorCommand_t;

typedef enum
{
    CLEAR_SCROP = 0x000, FILL_SCROP = 0x001, SCROLL_UP_SCROP = 0x002, SCROLL_DOWN_SCROP = 0x003
} spiScreenOp_t;

typedef enum
{
    RESET_AUOP,//Resets audio registers and things back to normal
    FREQ_SET0_AUOP, FREQ_SET1_AUOP, FREQ_SET2_AUOP
    //TODO add sweeping functionality and random (noise) mode
    
} spiAudioOp_t;

//PS/2 Commands
//TODO

#endif
