#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "regs.h"
#include "hex.h"
#include "uart.h"
#include "esp.h"

int main(int argc, char** argv) {

	void *virtual_base;
	int fd;
	int loop_count;
	int led_direction;
	int led_mask;
	char c;
	void *h2p_lw_led_addr;

	if (argc == 0) {
		printf("No arguments passed\n");
		return 1;
	} else if (argc != 2) {
		printf("Too many arguments passed\n");
		return 1;
	}
	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	h2p_lw_led_addr = virtual_base + (( unsigned long)(ALT_LWFPGASLVS_OFST + PIO_LED_BASE) & (unsigned long)(HW_REGS_MASK));

	hex_init(virtual_base);
	uart_init(virtual_base);
	// toggle the LEDs a bit
	char buffer[1024];
	char recvBuffer[1024];
	char *recvCur;
	char *cur;
	loop_count = 0;
	led_mask = 0x01;
	led_direction = 0; // 0: left to right direction
	while( 1 ) {
		uart_output();
		// control led
		*(uint32_t *)h2p_lw_led_addr = ~led_mask;

		if (argv[1][0] == '1') {
			printf("Setting Hex\n");
			hex_write(loop_count % 10);
		}
		else if (argv[1][0] == '2') {
			scanf("%s", buffer);
			
			while (*cur != '\0'){
				uart_write_data((unsigned int) *cur);
				cur++;
			}

			cur = buffer;
			// uart_read_data((unsigned int* )&c);

			// printf("Echo: %c\n", c);
		} else if (argv[1][0] == '3') {
			// printf("Reading Data\n");
			uart_read_data((unsigned int* )&c);
			printf("%c", c);
			// printf("Echo: %c\n", c);
		} else if (argv[1][0] == '4') {
			scanf("%s", buffer);
			cur = buffer;
			
			while (*cur != '\0') {
				uart_write_data((unsigned int) *cur);
				cur++;
			}

			uart_write_data((unsigned int) '\r');

			recvCur = recvBuffer;

			while (1) {
				uart_read_data((unsigned int *)recvCur);

				if (*recvCur - '0' >= 0 && *recvCur - '0' <= 9) {
					hex_write(*recvCur - '0');
				}
				printf("%c", *recvCur);

				if (*recvCur == '\n') {
					break;
				}
				recvCur++;
			}
		} else {
			printf("Running esp\n");

			run();
			return 1;
		}

		usleep(100 * 1000);

		// update led mask
		if (led_direction == 0){
			led_mask <<= 1;
			if (led_mask == (0x01 << (PIO_LED_DATA_WIDTH-1)))
				 led_direction = 1;
		} else {
			led_mask >>= 1;
			if (led_mask == 0x01){
				led_direction = 0;
			}
		}

		loop_count++;
	} // while

	// clean up our memory mapping and exit

	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}
