#ifndef UART_H
#define UART_H

#include "hps_0.h"

#define DATA_REGISTER *(volatile unsigned int *)(ESP_UART_BASE + 0x00)
#define CONTROL_REGISTER *(volatile unsigned int *)(ESP_UART_BASE + 0x00)

/* Data Register Masks */
#define DATA_MASK 0x7F
#define PE_MASK 0x200
#define RAVAIL_MASK 0xFF0000
#define RVALID_MASK 0x8000

/* Control Register Masks */
#define RE_MASK 0x1
#define WE_MASK 0x2
#define RI_MASK 0x100
#define WI_MASK 0x200
#define WSPACE_MASK 0xFF0000

/* UART Functions */
int write_data(char data);
int read_data(char *data);
