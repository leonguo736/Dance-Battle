#include <stdio.h>
#include "hwlib.h"
#include "socal/hps.h" 
#include "uart.h"
#include "regs.h"

volatile uint16_t *UART_RXDATA_REG;
volatile uint16_t *UART_TXDATA_REG;
volatile uint16_t *UART_STATUS_REG;
volatile uint16_t *UART_CONTROL_REG;

unsigned int uart_read_command(volatile uint16_t *reg, unsigned int mask);
int uart_write_command(volatile uint16_t *reg, unsigned int mask, unsigned int value);

unsigned int uart_read_command(volatile uint16_t *reg, unsigned int mask){
    return *reg &= mask;
}

int uart_write_command(volatile uint16_t *reg, unsigned int mask, unsigned int value){
    unsigned int write_val = *reg & ~mask;
    *reg = write_val | value;
    return 0;
}

void uart_init(void *virtual_base) {
    // Set the control register to enable the UART
    UART_RXDATA_REG   =  (uint16_t *)(virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_UART_BASE + UART_RXDATA_OFFSET) & (unsigned long)(HW_REGS_MASK)));
    UART_TXDATA_REG   =  (uint16_t *)(virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_UART_BASE + UART_TXDATA_OFFSET) & (unsigned long)(HW_REGS_MASK)));
    UART_STATUS_REG   =  (uint16_t *)(virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_UART_BASE + UART_STATUS_OFFSET) & (unsigned long)(HW_REGS_MASK)));
    UART_CONTROL_REG  =  (uint16_t *)(virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + ESP_UART_BASE + UART_CONTROL_OFFSET) & (unsigned long)(HW_REGS_MASK)));

    uart_output();
}

int uart_read_data(unsigned int *data) {
    // printf("uart_read_data - %x\n", *UART_STATUS_REG);
    // Poll until the previous bit has been shifted
    while (uart_read_command(UART_STATUS_REG, UART_RRDY_MASK) == 0); 

    // Read the data from the data register
    *data = uart_read_command(UART_RXDATA_REG, UART_RXDATA_MASK);

    return 0;
}

int uart_write_data(unsigned int value) {
    // printf("uart_write_data - %x\n", *UART_STATUS_REG);
    
     // Poll until the previous bit has been shifted
    while (uart_read_command(UART_STATUS_REG, UART_TRDY_MASK) == 0);
    // Write the data to the data register
    return uart_write_command(UART_TXDATA_REG, UART_TXDATA_MASK, value);
}

void uart_output(void) {
    printf("----- UART REG ADDRS -----\n");
    printf("%p | %p | %p | %p\n", UART_RXDATA_REG, UART_TXDATA_REG, UART_STATUS_REG, UART_CONTROL_REG);
    printf("----- UART REGS -----\n");
    printf("%x | %x | %x | %x\n", *UART_RXDATA_REG, *UART_TXDATA_REG, *UART_STATUS_REG, *UART_CONTROL_REG);
}