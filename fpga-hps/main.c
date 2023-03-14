#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define AVALON_IMG_WRITER_RGBGRAY_SPAN 64

#define MODE_ADDRESS            0
#define BUFF0_ADDRESS           1
#define BUFF1_ADDRESS           2
#define CONT_DOUBLE_BUFF_ADDRESS 3
#define BUFFER_SELECT_ADDRESS   4
#define START_CAPTURE_ADDRESS   5
#define STANDBY_ADDRESS         6
#define LAST_BUFFER_ADDRESS     7
#define DOWNSAMPLING_ADDRESS    8
#define IMAGE_COUNTER_ADDRESS   9

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

int main() {
	int fd;
	void *virtual_base;
    void *avalon_img_writer_base; 

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}
	printf("success open /dev/mem\n");

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	printf("success mmap\n");

	avalon_img_writer_base = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE ) & ( unsigned long)( HW_REGS_MASK ) );

	printf("success address at %p\n", avalon_img_writer_base);

	// int buffer_size = 640 * 480 * 4;
	// uint8_t *buff0 = (uint8_t *)mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
	// if (buff0 == MAP_FAILED) {
	// 	printf("ERROR: mmap() failed...\n");
	// 	close(fd);
	// 	return(1);
	// }
	// uint8_t *buff1 = (uint8_t *)mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	// if (buff1 == MAP_FAILED) {
	// 	printf("ERROR: mmap() failed...\n");
	// 	close(fd);
	// 	return(1);
	// }
	// printf("success allocate buffer0 and buffer1 at addresses %p %p\n", buff0, buff1);

	// pass address of buffers to IP
	volatile uint32_t *buff0_addr = (uint32_t *)(((unsigned long)avalon_img_writer_base + (unsigned long)(BUFF0_ADDRESS * 4)) & (unsigned long)(HW_REGS_MASK));
	volatile uint32_t *buff1_addr = (uint32_t *)(((unsigned long)avalon_img_writer_base + (unsigned long)(BUFF1_ADDRESS * 4)) & (unsigned long)(HW_REGS_MASK));

	*buff0_addr = (uint32_t)123; 
	*buff1_addr = (uint32_t)987; 
	printf("success pass address of buffers to IP, values %x %x\n", *buff0_addr, *buff1_addr);

	// // write to registers
	// *(uint32_t *)(avalon_img_writer_base + (MODE_ADDRESS * 4)) = 1; // continuous mode
	// *(uint32_t *)(avalon_img_writer_base + (CONT_DOUBLE_BUFF_ADDRESS * 4)) = 1; // continuous double buffer
	// *(uint32_t *)(avalon_img_writer_base + (BUFFER_SELECT_ADDRESS * 4)) = 0; // buffer0
	// *(uint32_t *)(avalon_img_writer_base + (START_CAPTURE_ADDRESS * 4)) = 1; // start capture

	// printf("success write to registers\n");

	// while (*(uint32_t *)(avalon_img_writer_base + (LAST_BUFFER_ADDRESS * 4)) == 0); 

	// printf("success standby\n");

	// // print buff0
	// for (int i = 0; i < 10; i++) {
	// 	printf("%d ", buff0[i]);
	// }
	// printf("\n");

	// clean up our memory mapping and exit
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}
