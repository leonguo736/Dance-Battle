#include "hex.h"

#include "regs.h"

#define HEX_SPLIT 4
uint32_t *h2p_hex3_hex0_addr;
uint32_t *h2p_hex5_hex4_addr;

char hex_values[10] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

void hex_init(void *virtual_base) {
  h2p_hex3_hex0_addr =
      (uint32_t *)(virtual_base +
                   ((unsigned long)(ALT_LWFPGASLVS_OFST + HEX3_HEX0_BASE) &
                    (unsigned long)(HW_REGS_MASK)));
  h2p_hex5_hex4_addr =
      (uint32_t *)(virtual_base +
                   ((unsigned long)(ALT_LWFPGASLVS_OFST + HEX5_HEX4_BASE) &
                    (unsigned long)(HW_REGS_MASK)));

  hex_clear(0);
}

void hex_write(uint8_t *hex, unsigned int value) { *hex = value; }

void hex_clear(unsigned int start) {
  for (unsigned i = start; i < MAX_HEXES; i++) {
    hex_write((i < HEX_SPLIT ? (uint8_t *)(h2p_hex3_hex0_addr)
                             : (uint8_t *)(h2p_hex5_hex4_addr)) +
                  (i % HEX_SPLIT),
              HEX_OFF);
  }
}

void display(int value) {
  int negative = value < 0;
  value = abs(value);

  unsigned i = 0;
  do {
    hex_write((i < HEX_SPLIT ? (uint8_t *)(h2p_hex3_hex0_addr)
                             : (uint8_t *)(h2p_hex5_hex4_addr)) +
                  (i % HEX_SPLIT),
              hex_values[value % 10]);
    value /= 10;
    i++;
  } while (value > 0 && i < MAX_HEXES);

  if (negative && i < MAX_HEXES) {
    hex_write((i < HEX_SPLIT ? (uint8_t *)(h2p_hex3_hex0_addr)
                             : (uint8_t *)(h2p_hex5_hex4_addr)) +
                  (i % HEX_SPLIT),
              HEX_DASH);
    i++;
  }

  hex_clear(i);
}
