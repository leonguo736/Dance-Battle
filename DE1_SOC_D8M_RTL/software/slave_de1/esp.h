#ifndef ESP_H
#define ESP_H

#include <stdbool.h>
#include <unistd.h>

#include "const.h"
#include "uart.h"

bool esp_init(int argc, char **argv);
void esp_run(void);

#endif