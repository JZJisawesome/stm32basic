# stm32basic

Basic interpreter (PS2 Keyboard and composite) using 2 stm32f103c8t6 mcus.

## Connections

Reset lines are tied together, pulled up, and active low (reset by pushing button)

### CPU

PA3:    USART2 RX pin, connected to Video USART2 TX pin (PA2)
PB12:   NSS2, connected to Video NSS2 (PB12) for SPI
PB13:   SCK2, connected to Video SCK2 (PB13) for SPI
PB14:   Active high Video ready line; pulled down internally; connected to Video PB14
PB15:   MOSI2, connected to Video MOSI2 (PB15) for SPI

### Video

PA2:USART2 TX pin, connected to CPU USART2 RX pin (PA3)
PA6:Composite video sync pin, connected to diode (see composite.h for circuit info)
PA7:Composite video pin (MOSI1), connected to diode (see composite.h for circuit info)
PA8:Audio channel 0 (connected to 1k resistor connected to buzzer and PB5 resistor)
PB4:PS/2 clock pin
PB5:Audio channel 1 (connected to 1k resistor connected to buzzer and PA8 resistor)
PB7:PS/2 data pin
PB12:NSS2, connected to CPU NSS2 (PB12) for SPI
PB13:SCK2, connected to CPU SCK2 (PB13) for SPI
PB14:Active high Video ready line; connected to CPU PB14
PB15:MOSI2, connected to CPU MOSI2 (PB15) for SPI

## Interrupt priorities

### CPU

N/A as of this moment

### Video

TIM4: 0b0000 (Max)
EXTI4: 0b0001

## DMA priorities

### CPU

SPI2 TX DMA: 1/4 priority (0b00)
UART RX DMA: 4/4 priority (0b11)

### Video

SPI1 TX DMA: 4/4 priority (0b11)
SPI2 RX DMA: 3/4 priority (0b10)

## Communication and Data Representation

### Processed PS/2 UART Data

If bit 7 = 0, then the data sent is ascii (the best possible translation from the ps/2 scan codes).
Shifting, caps lock, num lock, etc, is handled by the Video MCU
Things like delete, backspace, tap, etc that map to non-printing ascii characters are mapped as ascii
Things that do not will return special values (with bit 7 = 1)

TODO add a list

### SPI Commands (CPU->Video)

CPU internally pulls PB14 low, waits for Video to pull high to signal ready

Bits 15:9 are the major command
Bits 8:0 are data or for minor commands

Major commands
0x00: Character write (bits 6:0)
0x01: Screen operation based on bits 8:0
0x02: String write
0x03: Character position set
0x04: Line Draw
0x05: H line draw
0x06: V line draw
0x07: Polygon draw
0x08: Circle draw

0x0A: Character relative position set x
0x0B: Character relative position set y

TODO add more/finalize the rest

#### Regular command info

##### Screen operation (minor commands)

0x000: Screen clear
0x001: Screen fill
0x002: Screen character scroll up (8 pixels)
0x003: Screen character scroll down (8 pixels)

##### Character relative position set x and y

Bits 7:0 of command provide offset for new x or y position (twos complement) from current position (offset)

#### Multicommand info

Useful for commands that require > 9 bits of data to do things/want to be more efficient.
Will take 16 bits of info until they have enough (in addition to initial 9 bits), then will drop back to regular commands.

##### String write

Like multiple character writes, but more efficient (holding 2 characters per 16 bit commmand)
First command provides 1st character in bits 6:0.
Additional pairs of 2 characters are sent in following commands. The first is in bits 6:0, the second in 14:8
When a null byte is encountered, the command stops

##### Character position set

1st 8 bits (part of command) is x (characterX, so steps of 8 pixels)
2nd 8 bits is y (characterY, so steps of 8 pixels)

##### Line draw

Pixel coordinates are 9 bits. 1st x coordinate are sent with command (bits 8:0)
1st y coordinate is sent as bits 8:0 of next 16 transfer
2nd x coordinate is sent as bits 8:0 of next 16 transfer
2nd y coordinate is sent as bits 8:0 of next 16 transfer

##### H and V line draw

All data is 9 bits (8:0)
1st (part of command) is x
2nd is y
3rd is length (either horizontal or vertical)

##### Polygon draw

All data is 9 bits (8:0)
1st (part of command) is number of vertices (the number of pairs to send)
Pairs of coordinates are sent

The command connects lines between the vertices in the order they are sent, then connects the last pair to the first

##### Circle draw

All data is 9 bits (8:0)
1st (part of command) is x
2nd is y
3rd is radius

### BASIC Tokens (line_t.tokens)

TODO
