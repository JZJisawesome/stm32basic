#ifndef COMMUNICATION_DEFS_H
#define COMMUNICATION_DEFS_H

//SPI Commands
typedef enum
{
    CHAR_WRITE, SCREEN_OP, STRING_WRITE, CHAR_POS_SET, LINE_DRAW, HLINE_DRAW, VLINE_DRAW, POLY_DRAW,
    CIRCLE_DRAW, AUDIO_OP, CHAR_RELX_POS_SET, CHAR_RELY_POS_SET
} spiMajorCommand_t;

typedef enum
{
    CLEAR_SCROP, FILL_SCROP, SCROLL_UP_SCROP, SCROLL_DOWN_SCROP
} spiScreenOp_t;

typedef enum
{
    RESET_AUOP,//Resets audio registers and things back to normal
    FREQ_SET0_AUOP, FREQ_SET1_AUOP, FREQ_SET2_AUOP
    //TODO add sweeping functionality and random (noise) mode
    
} spiAudioOp_t;

//PS/2 Commands
typedef enum
{
    PS2_FN1 = 0x81, PS2_FN2, PS2_FN3, PS2_FN4, PS2_FN5, PS2_FN6, PS2_FN7, PS2_FN8, PS2_FN9, PS2_FN10, PS2_FN11, PS2_FN12,
    PS2_INSERT, PS2_HOME, PS2_PGUP, PS2_END, PS2_PGDOWN, PS2_UP, PS2_DOWN, PS2_LEFT, PS2_RIGHT//TODO add more
} ps2Code_t;//Interpret as ascii if msb not set

#endif
