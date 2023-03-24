#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "altera_avalon_uart_regs.h"
#include "system.h"
#include "uart.h"
#include "const.h"

/* rxdata register */
#define UART_RXDATA_OFFSET 0x00
#define UART_RXDATA_MASK 0xFF

/* txdata register */
#define UART_TXDATA_OFFSET 0x01
#define UART_TXDATA_MASK 0xFF

/* status register */
#define UART_STATUS_OFFSET 0x08
#define UART_STATUS_MASK 0xFFFF
#define UART_PE_MASK 0x1
#define UART_FE_MASK 0x2
#define UART_BRK_MASK 0x4
#define UART_ROE_MASK 0x8
#define UART_TOE_MASK 0x10
#define UART_TMT_MASK 0x20
#define UART_TRDY_MASK 0x40
#define UART_RRDY_MASK 0x80
#define UART_E_MASK 0x100
#define UART_DCTS_MASK 0x400
#define UART_CTS_MASK 0x800
#define UART_EOP_MASK 0x1000

/* control register */
#define UART_CONTROL_OFFSET 0x12
#define UART_CONTROL_MASK 0xFFFF
#define UART_IPE_MASK 0x1
#define UART_IFE_MASK 0x2
#define UART_IBRK_MASK 0x4
#define UART_IROE_MASK 0x8
#define UART_ITOE_MASK 0x10
#define UART_ITMT_MASK 0x20
#define UART_ITRDY_MASK 0x40
#define UART_IRRDY_MASK 0x80
#define UART_IE_MASK 0x100
#define UART_TRBK_MASK 0x200
#define UART_IDCTS_MASK 0x400
#define UART_RTS_MASK 0x800
#define UART_IEOP_MASK 0x1000

// Private Variables
volatile uint16_t *UART_RXDATA_REG;
volatile uint16_t *UART_TXDATA_REG;
volatile uint16_t *UART_STATUS_REG;
volatile uint16_t *UART_CONTROL_REG;

void uart_write_data(char *data);
int uart_read_byte(uint8_t *data);
void uart_write_byte(uint8_t data);

// Private Function Prototypes
// uint16_t uart_read_command(volatile uint16_t *reg, uint16_t mask);
// void uart_write_command(volatile uint16_t *reg, uint16_t mask, uint16_t
// value);

// // Private Function Definitions
// uint16_t uart_read_command(volatile uint16_t *reg, uint16_t mask) {
//   return *reg &= mask;
// }

// void uart_write_command(volatile uint16_t *reg, uint16_t mask,
//                         uint16_t value) {
//   uint16_t write_val = *reg & ~mask;
//   *reg = write_val | value;
// }

int uart_read_byte(uint8_t *data) {
#ifdef DEBUG_SEVERE
  printf("uart_read_byte - %x \n", IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE));
#endif

  if ((IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & UART_ROE_MASK) == 1) {
    // if (uart_read_command(UART_STATUS_REG, UART_ROE_MASK) != 0) {
    printf("uart_read_data - ROE\n");
  }
  // Poll until the previous bit has been shifted
  while ((IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & UART_RRDY_MASK) == 0)
    ;
  // while (uart_read_command(UART_STATUS_REG, UART_RRDY_MASK) == 0);

  // Read the data from the data register
  *data = IORD_ALTERA_AVALON_UART_RXDATA(UART_0_BASE);
  // *data = uart_read_command(UART_RXDATA_REG, UART_RXDATA_MASK);

  if ((IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & UART_ROE_MASK) == 1) {
    // if (uart_read_command(UART_STATUS_REG, UART_ROE_MASK) != 0) {
    printf("uart_read_data - ROE\n");
  }

  return 0;
}

void uart_write_byte(uint8_t value) {
#ifdef DEBUG_SEVERE
  printf("uart_write_byte - %c | %x\n", value,
         IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE));
#endif

  // Poll until the previous bit has been shifted
  while ((IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & UART_TRDY_MASK) == 0)
    ;
  // while (uart_read_command(UART_STATUS_REG, UART_TRDY_MASK) == 0);

  // Write the data to the data register
  IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, value);
  // uart_write_command(UART_TXDATA_REG, UART_TXDATA_MASK, value);
}

// Global Function Definitions
void uart_init(void) {
  // Set the control register to enable the UART
  UART_RXDATA_REG = (uint16_t *)IOADDR_ALTERA_AVALON_UART_RXDATA(UART_0_BASE);
  UART_TXDATA_REG = (uint16_t *)IOADDR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE);
  UART_STATUS_REG = (uint16_t *)IOADDR_ALTERA_AVALON_UART_STATUS(UART_0_BASE);
  UART_CONTROL_REG = (uint16_t *)IOADDR_ALTERA_AVALON_UART_CONTROL(UART_0_BASE);

  // UART_RXDATA_REG   =  (volatile uint16_t *)UART_0_BASE + UART_RXDATA_OFFSET;
  // UART_TXDATA_REG   =  (volatile uint16_t *)UART_0_BASE + UART_TXDATA_OFFSET;
  // UART_STATUS_REG   =  (volatile uint16_t *)UART_0_BASE + UART_STATUS_OFFSET;
  // UART_CONTROL_REG  =  (volatile uint16_t *)UART_0_BASE +
  // UART_CONTROL_OFFSET; Clear status register
  IOWR_ALTERA_AVALON_UART_STATUS(UART_0_BASE, 0);
  // *UART_STATUS_REG = 0;

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

  // Read from UART and store in buffer
  while (*str != '\n' && actualLen < len) {
    uart_read_byte((uint8_t *)str);
    str += *str != '\n';
    actualLen += *str != '\n';
  }

  *(str - (actualLen == len)) = '\0';

  if (actualLen < len) {
    return actualLen;
  } else {
    printf("Buffer Overflow - %d\n", actualLen);
    return -1;
  }
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
  printf("%x | %x | %x | %x\n", IORD_ALTERA_AVALON_UART_RXDATA(UART_0_BASE),
         IORD_ALTERA_AVALON_UART_TXDATA(UART_0_BASE),
         IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE),
         IORD_ALTERA_AVALON_UART_CONTROL(UART_0_BASE));
  // printf("%x | %x | %x | %x\n", uart_read_command(UART_RXDATA_REG,
  // UART_RXDATA_MASK),
  //       uart_read_command(UART_TXDATA_REG, UART_TXDATA_MASK),
  //       uart_read_command(UART_STATUS_REG, UART_STATUS_MASK),
  //       uart_read_command(UART_CONTROL_REG, UART_CONTROL_MASK));
#endif
}