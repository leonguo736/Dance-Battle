#ifndef ESP_H
#define ESP_H

#include <unistd.h>

#include "const.h"
#include "uart.h"

void esp_init(int argc, char **argv);
void esp_run(void);

#endif