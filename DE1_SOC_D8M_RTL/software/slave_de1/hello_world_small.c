/*
 * "Small Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It requires a STDOUT  device in your system's hardware.
 *
 * The purpose of this example is to demonstrate the smallest possible Hello
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete
 * description.
 *
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 *
 *    - Set the Optimization Level to -Os
 *
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 *
 *    - Set the Optimization Level to -Os
 *
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION
 *      This removes software exception handling, which means that you cannot
 *      run code compiled for Nios II cpu with a hardware multiplier on a core
 *      without a the multiply unit. Check the Nios II Software Developers
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks
 *      support for buffering, file IO, floating point and getch(), etc.
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "sys/alt_stdio.h"
#include "system.h"
#include "esp.h"

#define NUM_POINT_FINDERS 2
uint32_t coords[NUM_POINT_FINDERS];
volatile uint32_t *camera_base = (uint32_t *)COORDS_SLAVE_0_BASE;

void writeThresholds(int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow,
                     uint8_t crHigh) {
  *(camera_base + i) = (cbLow << 24) | (cbHigh << 16) | (crLow << 8) | crHigh;
}

void updateCoords() {
  for (int i = 0; i < NUM_POINT_FINDERS; i++) {
    coords[i] = *(camera_base + i);
  }
}

int main(int argc, char **argv) {
  const int __programNumber__ = 420;
  printf("\n === Program start number: %i === \n", __programNumber__);

  writeThresholds(0, 0, 255, 0, 255);
  writeThresholds(0, 146, 160, 61, 85);  // Leon's Blue
  writeThresholds(1, 64, 93, 156, 175);  // Kerry's Dark Red

  uart_init();
  uart_output();

  if (!esp_init(argc, argv)) {
    while(1)
      ;
    return;
  };

  esp_run();
  // pid_t pid = fork();

  // if (pid == -1) {
  //   printf("Fork failed\n");
  //   while(1)
  //     ;
  //   return 1;
  // } else if (pid == 0) {
  //   esp_run();
  // } else {
  //   printf("Parent Process\n");
  //   while (1) {
  //     updateCoords();
  //     for (int i = 0; i < NUM_POINT_FINDERS; i++) {
  //       uint32_t raw_coords = coords[i];
  //       uint16_t smallUpBigDown = raw_coords >> 16;
  //       uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
  //       printf("%i: (%i, %i), ", i, smallLeftBigRight, smallUpBigDown);
  //     }
  //     printf("\n");
  //   }
  // }

  printf("\n === Program end number: %i === \n", __programNumber__);
  while (1)
    ;
  return 0;
}

// Top right: 120, 430
// Top left 100, 200
// Bottom right: 300, 430
