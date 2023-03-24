#ifndef UART_H
#define UART_H

#include <stdint.h>

/* constants */
#define UART_BUFFER_SIZE 1024

/* UART Functions */
void uart_init(void);
void uart_output(void);
char *uart_wait_for_messages(char **messages, unsigned int numMessages);
void uart_send_command(char *cmd, char **args, unsigned int numArgs);
int uart_read_data(char *data, int len);

#endif