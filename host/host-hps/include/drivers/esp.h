#ifndef ESP_H
#define ESP_H

#include <stdlib.h>
#include <unistd.h>

#include "const.h"
#include "hwlib.h"  // has uintX_t defines
#include "uart.h"

#define ESP_BUFFER_SIZE 1024
#define ESP_CONNECT_THRESHOLD 10

/* Global Variables */
extern bool esp_ready;
extern bool esp_failed;

/* Function Prototypes */

/*
 * Resets the ESP module
 */
void esp_reset(void);

/*
 * Initializes the ESP
 */
void esp_init(char *server_ip);

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

/*
 * Called when backend connection closes
 */
void esp_close(char *server_ip);

#endif