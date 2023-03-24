#ifndef UART_H
#define UART_H

#include <stdint.h>

/* constants */
#define UART_BUFFER_SIZE 1024

/* UART Functions */
void uart_init(void);
void uart_output(void);

#endif