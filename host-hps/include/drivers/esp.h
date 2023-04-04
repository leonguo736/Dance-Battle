#ifndef ESP_H
#define ESP_H

#include <unistd.h>
#include <stdlib.h>

#include "const.h"
#include "uart.h"

#define ESP_BUFFER_SIZE UART_BUFFER_SIZE
// Global Variables
extern bool esp_connected;

/*
 * Initializes the ESP
 */
bool esp_init(int argc, char **argv);

/*
 * Reads a string from the ESP
 * len - length of the string
 * Returns a string that must be free'd by the caller
 * Returns NULL if there was an error or no data was read
 */
char *esp_read(unsigned int *len);

/*
 * Writes a string to the ESP
 * data - string to write
 */
void esp_write(char *data);

#endif