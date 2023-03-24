#include <stdio.h>
#include <unistd.h>

#include "esp.h"
#include "hex.h"
#include "regs.h"
#include "uart.h"

int main(int argc, char **argv) {
  // Regs
  void *virtual_base;

  // LEDs
  int loop_count;
  int led_direction;
  int led_mask;
  void *h2p_lw_led_addr;

  // ESP
  char sendBuffer[UART_BUFFER_SIZE];
  char recvBuffer[UART_BUFFER_SIZE];
  char* recvPtr;

  if (regs_init(&virtual_base)) {
    printf("Failed to initialize virtual base\n");
    return 1;
  }

  h2p_lw_led_addr =
      virtual_base + ((unsigned long)(ALT_LWFPGASLVS_OFST + LEDS_BASE) &
                      (unsigned long)(HW_REGS_MASK));

  hex_init(virtual_base);
  uart_init(virtual_base);

  // toggle the LEDs a bit
  loop_count = 0;
  led_mask = 0x01;
  led_direction = 0;  // 0: left to right direction

  printf("Init complete\n");
  // Main while loop
  while (true) {
    // control led
    *(uint32_t *)h2p_lw_led_addr = ~led_mask;

    if (argc == 2) {
      if (argv[1][0] == '1') {
        printf("Setting Hex: %d\n", loop_count);
        display(loop_count);
      } else if (argv[1][0] == '2') {
        unsigned int len = 0;
        recvPtr = recvBuffer;

        while (*recvPtr != '\n') {
          uart_read_byte(recvPtr);

          if (*recvPtr == '\n') {
            break;
          }

          recvPtr++;
          len++;
        }

        *recvPtr = '\0';
        printf("[%d] %s\n", len, recvBuffer);
      } else if (argv[1][0] == '3') {
        unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
        printf("[%d] %s\n", len, recvBuffer);
      } else if (argv[1][0] == '4') {
        printf("Enter string to send: ");
        scanf("%s", sendBuffer);

        uart_write_data(sendBuffer);

        uart_output();
        unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
        printf("[%d] %s\n", len, recvBuffer);
      }
    } else {
      run(argc, argv);
      return 1;
    }

    // usleep(100 * 1000);

    // update led mask
    if (led_direction == 0) {
      led_mask <<= 1;
      if (led_mask == (0x01 << (LEDS_DATA_WIDTH - 1))) led_direction = 1;
    } else {
      led_mask >>= 1;
      if (led_mask == 0x01) {
        led_direction = 0;
      }
    }

    loop_count++;
  }  // while

  // clean up our memory mapping and exit
  return regs_close(virtual_base);
}
