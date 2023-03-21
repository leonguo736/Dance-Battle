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

#include "sys/alt_stdio.h"
#include "system.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define NUM_POINT_FINDERS 1
volatile uint32_t *camera_base = COORDS_SLAVE_0_BASE;
static uint32_t thresholds[NUM_POINT_FINDERS];

/* arr is mutated to contain the coordinates the points
 * len is set to the size of arr
 *
 * (x,y) = (0,0) is at top left of VGA monitor
 * x = arr[i] & 0xFFFF;
 * y = arr[i] >> 16;
 */
void getFrame(uint32_t *arr, int *len)
{
	arr = (uint32_t *)malloc(NUM_POINT_FINDERS * sizeof(uint32_t));
	*len = NUM_POINT_FINDERS;

	uint32_t raw_coords;
	for (int i = 0; i < NUM_POINT_FINDERS; i++)
	{
		raw_coords = *(camera_base + i);
		arr[i] = raw_coords;
		//	  uint16_t smallUpBigDown = raw_coords >> 16;
		//	  uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
		//	  printf("smallUpBigDown: %i, smallLeftBigRight: %i\n", smallUpBigDown, smallLeftBigRight);
	}
}

// sample code for Bell
void printCoords()
{
	int len;
	uint32_t *arr = NULL;
	while (1)
	{
		getFrame(arr, &len);
		for (int i = 0; i < len; i++)
		{
			uint32_t raw_coords = arr[i];
			uint16_t x = raw_coords & 0xFFFF;
			uint16_t y = raw_coords >> 16;
			printf("x: %i, y: %i\n", x, y);
		}
	}
	free(arr);
}

void printThresholds(int i)
{
    printf("Thresholds: crLow=%d crHigh=%d cbLow=%d cbHigh=%d\n",
        (thresholds[i] >> 24) & 0xFF, (thresholds[i] >> 16) & 0xFF,
        (thresholds[i] >> 8) & 0xFF, thresholds[i] & 0xFF);
}

void writeThresholds(int i)
{
	*(camera_base + i) = thresholds[i];
}

char getInputLetter() {
    char c;
    while ((c = getchar()) == '\n'); // consume newline characters
    while (getchar() != '\n'); // clear input buffer
    return c;
}

void updateThresholds() {
	char c;
	int index = 0;
	while (1) {
	    printf("Modifying point %i, press a button on keyboard", index);
		c = getInputLetter();
		printf("%c\n", c); 
		switch (c) {
			case 'b': return; // break
			case 'B': return; // break
			case 't': index = (index + 1) % NUM_POINT_FINDERS; break; // increment index
			case 'g': index = (index - 1) % NUM_POINT_FINDERS; break; // decrement index
			case 'q': thresholds[index] += 1 << 8; break; // increment cbLow
			case 'w': thresholds[index] += 1; break; // increment cbHigh
			case 'e': thresholds[index] += 1 << 24; break; // increment crLow
			case 'r': thresholds[index] += 1 << 16; break; // increment crHigh
			case 'a': thresholds[index] -= 1 << 8; break; // decrement cbLow
			case 's': thresholds[index] -= 1; break; // decrement cbHigh
			case 'd': thresholds[index] -= 1 << 24; break; // decrement crLow
			case 'f': thresholds[index] -= 1 << 16; break; // decrement crHigh
			case '1': thresholds[index] += 5 << 8; break; // increment cbLow
			case '2': thresholds[index] += 5; break; // increment cbHigh
			case '3': thresholds[index] += 5 << 24; break; // increment crLow
			case '4': thresholds[index] += 5 << 16; break; // increment crHigh
			case 'z': thresholds[index] -= 5 << 8; break; // decrement cbLow
			case 'x': thresholds[index] -= 5; break; // decrement cbHigh
			case 'c': thresholds[index] -= 5 << 24; break; // decrement crLow
			case 'v': thresholds[index] -= 5 << 16; break; // decrement crHigh
			case '!': thresholds[index] += 50 << 8; break; // increment cbLow
			case '@': thresholds[index] += 50; break; // increment cbHigh
			case '#': thresholds[index] += 50 << 24; break; // increment crLow
			case '$': thresholds[index] += 50 << 16; break; // increment crHigh
			case 'Z': thresholds[index] -= 50 << 8; break; // decrement cbLow
			case 'X': thresholds[index] -= 50; break; // decrement cbHigh
			case 'C': thresholds[index] -= 50 << 24; break; // decrement crLow
			case 'V': thresholds[index] -= 50 << 16; break; // decrement crHigh
			case 'Q': thresholds[index] += 10 << 8; break; // increment cbLow
			case 'W': thresholds[index] += 10; break; // increment cbHigh
			case 'E': thresholds[index] += 10 << 24; break; // increment crLow
			case 'R': thresholds[index] += 10 << 16; break; // increment crHigh
			case 'A': thresholds[index] -= 10 << 8; break; // decrement cbLow
			case 'S': thresholds[index] -= 10; break; // decrement cbHigh
			case 'D': thresholds[index] -= 10 << 24; break; // decrement crLow
			case 'F': thresholds[index] -= 10 << 16; break; // decrement crHigh
			default: break;
		}
		writeThresholds(index);
		printThresholds(index);
	}
}

int main()
{
	const int __programNumber__ = 420;
	printf("Program start number: %i\n", __programNumber__);

	//	printCoords();

	uint8_t cbLow = 121, cbHigh = 130, crLow = 120, crHigh = 130;
	thresholds[0] = (crLow << 24) | (crHigh << 16) | (cbLow << 8) | cbHigh;
	writeThresholds(0);

	updateThresholds();

	printf("Program end number: %i\n", __programNumber__);
	return 0;
}

// Top right: 120, 430
// Top left 100, 200
// Bottom right: 300, 430
