#ifndef UART_H
#define UART_H

#include <stdint.h>

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
#define UART_CONTROL_OFFSET 0x012
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

/* constants */
#define UART_BUFFER_SIZE 1024
/* UART Functions */
void uart_init(void);
void uart_write_data(char *data);
int uart_read_data(char *data, int len);
int uart_read_byte(uint8_t *data);
void uart_write_byte(uint8_t data);
char *uart_wait_for_messages(char **messages, unsigned int numMessages);
void uart_send_command(char *cmd, char **args, unsigned int numArgs);
void uart_output(void);

#endif