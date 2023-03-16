#include "hex.h"
#include "regs.h"

uint8_t* h2p_hex_addr;

char hex_values[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void hex_init(void* virtual_base) {
    h2p_hex_addr = (uint8_t *)(virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + HEX0_BASE) & (unsigned long)(HW_REGS_MASK)));
}

void hex_write(unsigned int value) {
    *h2p_hex_addr = hex_values[value] ^ INVERT;
}