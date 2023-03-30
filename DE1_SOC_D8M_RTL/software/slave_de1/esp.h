#ifndef ESP_H
#define ESP_H

#include <stdbool.h>

/*
 * Initializes the ESP
 */
bool esp_init(int argc, char **argv);

/* 
 * Reads a string from the ESP
 * len - length of the string
 * Returns the string that must be free'd by the caller
 */
char *esp_read(unsigned int *len);


void esp_run(void);

#endif