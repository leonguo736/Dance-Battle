#ifndef UART_H
#define UART_H

#include <stdint.h>

/* constants */
#define UART_BUFFER_SIZE 1024

/* UART Functions */
/*
 * Initialize the UART
 */
void uart_init(void);

/*
 * Outputs UART Register information 
 */
void uart_output(void);

/*
 * Blocks until a of the messages are received from the UART
 * messages - array of messages to scan for
 * Returns the message that was received
 */
char *uart_wait_for_messages(char **messages, unsigned int numMessages);

/*
 * Sends a UART command to the ESP8266
 * cmd - command to send
 * args - arguments to send
 */
void uart_send_command(char *cmd, char **args, unsigned int numArgs);

/*
 * Reads a string from the UART
 * data - buffer to store the data
 * len - length of the buffer
 * Returns length of the data
 */
int uart_read_data(char *data, int len);

#endif