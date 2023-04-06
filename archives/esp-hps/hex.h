#ifndef HEX_H
#define HEX_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "const.h"

#define INVERT 0xFFFFFFFF
#define MAX_HEXES 6

#define HEX_OFF 0b00000000
#define HEX_ON 0b11111111
#define HEX_DASH 0b01000000

void hex_init(void* virtual_base);
void hex_write(uint8_t* hex, unsigned int value);
void hex_clear(unsigned int start);
void display(int value);

#endif