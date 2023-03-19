
#include "uart.h"

#include <stdio.h>

#include "hwlib.h"
#include "regs.h"
#include "socal/hps.h"

// Private Variables
volatile uint16_t *UART_RXDATA_REG;
volatile uint16_t *UART_TXDATA_REG;
volatile uint16_t *UART_STATUS_REG;
volatile uint16_t *UART_CONTROL_REG;

// Private Function Prototypes
unsigned int uart_read_command(volatile uint16_t *reg, unsigned int mask);
void uart_write_command(volatile uint16_t *reg, unsigned int mask, unsigned int value);
int uart_read_byte(unsigned int *data);
void uart_write_byte(unsigned int data);

// Private Function Definitions
unsigned int uart_read_command(volatile uint16_t *reg, unsigned int mask) {
  return *reg &= mask;
}

void uart_write_command(volatile uint16_t *reg, unsigned int mask,
                        unsigned int value) {
  uint16_t write_val = *reg & ~mask;
  *reg = write_val | value;
}

int uart_read_byte(unsigned int *data) {
  // printf("uart_read_data - %x\n", *UART_STATUS_REG);
  // Poll until the previous bit has been shifted
  while (uart_read_command(UART_STATUS_REG, UART_RRDY_MASK) == 0)
    ;

  // Read the data from the data register
  *data = uart_read_command(UART_RXDATA_REG, UART_RXDATA_MASK);

  // Check for ROE here
  return 0;
}

void uart_write_byte(unsigned int value) {
  // printf("uart_write_data - %x\n", *UART_STATUS_REG);

  // Poll until the previous bit has been shifted
  while (uart_read_command(UART_STATUS_REG, UART_TRDY_MASK) == 0)
    ;
  // Write the data to the data register
  uart_write_command(UART_TXDATA_REG, UART_TXDATA_MASK, value);
}

// Global Function Definitions
void uart_init(void *virtual_base) {
  // Set the control register to enable the UART
  UART_RXDATA_REG =
      (uint16_t *)(virtual_base +
                   ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_RS232_UART_BASE +
                                    UART_RXDATA_OFFSET) &
                    (unsigned long)(HW_REGS_MASK)));
  UART_TXDATA_REG =
      (uint16_t *)(virtual_base +
                   ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_RS232_UART_BASE +
                                    UART_TXDATA_OFFSET) &
                    (unsigned long)(HW_REGS_MASK)));
  UART_STATUS_REG =
      (uint16_t *)(virtual_base +
                   ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_RS232_UART_BASE +
                                    UART_STATUS_OFFSET) &
                    (unsigned long)(HW_REGS_MASK)));
  UART_CONTROL_REG =
      (uint16_t *)(virtual_base +
                   ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_RS232_UART_BASE +
                                    UART_CONTROL_OFFSET) &
                    (unsigned long)(HW_REGS_MASK)));

  uart_output();
}

void uart_write_data(char *str) {
  while (*str != '\0') {
    uart_write_byte(*str);
    str++;
  }
  uart_write_byte('\r');
}

int uart_read_data(char *str, int len) {
  unsigned int i = 0;

  // Clear buffer
  for (i = 0; i < len; i++) {
    str[i] = '\0';
  }

  // Read from UART and store in buffer
  while (*str != '\n') {
    uart_read_byte((unsigned int *)str);

    if (*str == '\n') {
      break;
    }
    str++;
    i++;
  }
  *str = '\0';
  return i;
  // if (i < len) {
  //   *(str - 1) = '\0';
  //   return i;
  // } else {
  //   printf("Buffer Overflow - %d\n", i);
  //   return -1;
  // }
}

void uart_output(void) {
  printf("----- UART REG ADDRS -----\n");
  printf("%p | %p | %p | %p\n", UART_RXDATA_REG, UART_TXDATA_REG,
         UART_STATUS_REG, UART_CONTROL_REG);

  printf("----- UART REGS -----\n");
  printf("%x | %x | %x | %x\n", *UART_RXDATA_REG, *UART_TXDATA_REG,
         *UART_STATUS_REG, *UART_CONTROL_REG);

  printf("----- UART REGS COMMANDS -----\n");
  printf("%x | %x | %x | %x\n", uart_read_command(UART_RXDATA_REG, UART_RXDATA_MASK),
         uart_read_command(UART_TXDATA_REG, UART_TXDATA_MASK),
         uart_read_command(UART_STATUS_REG, UART_STATUS_MASK),
         uart_read_command(UART_CONTROL_REG, UART_CONTROL_MASK));
}