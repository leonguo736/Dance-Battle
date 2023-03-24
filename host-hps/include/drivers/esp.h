#ifndef ESP_H
#define ESP_H

#include <unistd.h>
#include <stdlib.h>

#include "const.h"
#include "uart.h"

// Global Variables
extern bool esp_connected;


// Function Prototypes
bool esp_init(int argc, char** argv);
void esp_run(void);

#endif