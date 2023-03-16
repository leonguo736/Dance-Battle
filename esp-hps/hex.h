#ifndef HEX_H
#define HEX_H

#define INVERT 0xFFFFFFFF

void hex_init(void *virtual_base);
void hex_write(unsigned int value);

#endif