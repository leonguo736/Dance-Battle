
#include "uart.h"

#include <stdio.h>
#include <string.h>

#include "hps_0.h"
#include "hwlib.h"
#include "regs.h"
#include "socal/hps.h"

// Private Variables
volatile uint16_t *UART_RXDATA_REG;
volatile uint16_t *UART_TXDATA_REG;
volatile uint16_t *UART_STATUS_REG;
volatile uint16_t *UART_CONTROL_REG;

// Private Function Prototypes
/*
 * Returns whether the UART is ready to read data
 */
int uart_read_ready(void);

/*
 * Writes a string to the UART
 * data - the string to write
 */
void uart_write_data(char *data);

/*
 * Reads a byte from the UART and stores it in the data pointer.
 * Blocks until a byte is available
 * Returns 0 on success, -1 on failure
 */
int uart_read_byte(uint8_t *data);

/*
 * Reads from UART register
 * reg - the register to read from
 * mask - the mask to apply to the register
 * Returns the value of the register
 */
uint16_t uart_read_command(volatile uint16_t *reg, uint16_t mask);

/*
 * Writes to UART register
 * reg - the register to write to
 * mask - the mask to apply to the register
 * value - the value to write to the register
 */
void uart_write_command(volatile uint16_t *reg, uint16_t mask, uint16_t value);

// Private Function Definitions
uint16_t uart_read_command(volatile uint16_t *reg, uint16_t mask) {
  return *reg &= mask;
}

void uart_write_command(volatile uint16_t *reg, uint16_t mask, uint16_t value) {
  uint16_t write_val = *reg & ~mask;
  *reg = write_val | value;
}

int uart_read_ready(void) {
  return (uart_read_command(UART_STATUS_REG, UART_RRDY_MASK));
}

int uart_read_byte(uint8_t *data) {
  // printf("uart_read_byte - %x \n", *UART_STATUS_REG);

  if (uart_read_command(UART_STATUS_REG, UART_ROE_MASK) != 0) {
    printf("uart_read_data - ROE\n");
  }

  // Poll until the previous bit has been shifted
  while (!uart_read_ready())
    ;

  // Read the data from the data register
  *data = uart_read_command(UART_RXDATA_REG, UART_RXDATA_MASK);

  if (uart_read_command(UART_STATUS_REG, UART_ROE_MASK) != 0) {
    printf("uart_read_data - ROE\n");
  }

  return 0;
}

void uart_write_byte(uint8_t value) {
  // printf("uart_write_byte - %c | %x\n", value, *UART_STATUS_REG);

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

  // Clear status register
  *UART_STATUS_REG = 0;

  uart_output();
}

void uart_write_data(char *str) {
  while (*str != '\0') {
    uart_write_byte((uint16_t)*str);
    str++;
  }
  uart_write_byte('\r');
}

int uart_read_data(char *str, int len) {
  unsigned int actualLen = 0;

  if (uart_read_ready()) {  // Clear buffer
    for (unsigned int i = 0; i < len; i++) {
      str[i] = '\0';
    }

    // Read from UART and store in buffer
    while (*str != '\n' && actualLen < len) {
      uart_read_byte((uint8_t *)str);
      str += *str != '\n';
      actualLen += *str != '\n';
    }

    *str = '\0';

    if (actualLen >= len) {
      printf("Buffer Overflow - %d\n", actualLen);
      actualLen = -1;
    }
  } 

  return actualLen;
}

char *uart_wait_for_messages(char **messages, unsigned int numMessages) {
#ifdef DEBUG
  printf("Waiting for messages: ");
  for (unsigned int i = 0; i < numMessages; i++) {
    printf("%s, ", messages[i]);
  }
  printf("\n");
#endif
  char recvBuffer[UART_BUFFER_SIZE];
  char *yieldedMessage = NULL;
  do {
    uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    for (unsigned int i = 0; i < numMessages && !yieldedMessage; i++) {
      yieldedMessage =
          strcmp(recvBuffer, messages[i]) == 0 ? messages[i] : NULL;
    }
  } while (!yieldedMessage);
  return yieldedMessage;
}

void uart_send_command(char *cmd, char **args, unsigned int numArgs) {
  char sendBuffer[UART_BUFFER_SIZE];
#ifdef DEBUG 
  printf("Parsing Command: %s |", cmd);
#endif
  sprintf(sendBuffer, "%s", cmd);
  for (unsigned int i = 0; i < numArgs; i++) {
    #ifdef DEBUG
      printf(" %s,", args[i]);
    #endif
    sprintf(sendBuffer, "%s%s ", sendBuffer, args[i]);
  }

  #ifdef DEBUG
    printf("\nSending Command: %s\n", sendBuffer);
  #endif
  uart_write_data(sendBuffer);
}

void uart_output(void) {
  #ifdef DEBUG
    printf("----- UART REG ADDRS -----\n");
    printf("%p | %p | %p | %p\n", UART_RXDATA_REG, UART_TXDATA_REG,
           UART_STATUS_REG, UART_CONTROL_REG);

    printf("----- UART REGS -----\n");
    printf("%x | %x | %x | %x\n", *UART_RXDATA_REG, *UART_TXDATA_REG,
           *UART_STATUS_REG, *UART_CONTROL_REG);

    printf("----- UART REGS COMMANDS -----\n");
    printf("%x | %x | %x | %x\n",
           uart_read_command(UART_RXDATA_REG, UART_RXDATA_MASK),
           uart_read_command(UART_TXDATA_REG, UART_TXDATA_MASK),
           uart_read_command(UART_STATUS_REG, UART_STATUS_MASK),
           uart_read_command(UART_CONTROL_REG, UART_CONTROL_MASK));
  #endif
}