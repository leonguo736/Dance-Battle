#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

#define SDRAM_BASE ( HPS_0_AXI_SDRAM_BASE )
#define SDRAM_SPAN 0x03FFFFFF
#define BUFF0_OFFSET 0x100000
#define BUFF1_OFFSET 0x200000

int main() {
	int fd;
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	void *virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );
	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed for virtual_base...\n" );
		close( fd );
		return( 1 );
	}
	printf("virtual_base: %p\n", virtual_base); 

	void *sdram_virtual_base = mmap( NULL, SDRAM_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, SDRAM_BASE );
	if( sdram_virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed for sdram_virtual_base...\n" );
		close( fd );
		return( 1 );
	}

	unsigned int *buff0_addr = (unsigned int *)((char *)sdram_virtual_base + BUFF0_OFFSET); 
	printf("buff0_addr: %p\n", buff0_addr);
	unsigned int *buff1_addr = (unsigned int *)((char *)sdram_virtual_base + BUFF1_OFFSET);
	printf("buff1_addr: %p\n", buff1_addr);

// --Avalon slave

//   --Internal register address map 
//     --Mode permits to select between 2 modes: 
// 	   --A 0 in this register (default) selects "SINGLE SHOT" mode.
// 		--A 1 in this register selects "CONTINUOUS" mode.
// 	 constant MODE_ADDRESS            : integer := 0;
// 	 constant BUFF0_ADDRESS           : integer := 1;
// 	 constant BUFF1_ADDRESS           : integer := 2;
// 	 --In continuous mode selects saving in 1 buffer (write a 0 here, 
// 	 --the default after reset) or 2 buffers (write a 1 here)
// 	 constant CONT_DOUBLE_BUFF_ADDRESS: integer := 3;
// 	 --Number of the buffer where you wanna write next image (0 or 1)
// 	 --In CONTINUOUS mode writing in 2 buffers it is ignored cause
// 	 --the component alternates buff0 and buff1.
// 	 constant BUFFER_SELECT_ADDRESS  : integer  := 4;
// 	 -- Start the capure of image (SINGLE SHOT) or images (CONTINUOUS).
// 	 -- In SINGLE SHOT write a 1 here to save 1 image to memory. It 
// 	 -- automatically goes to 0 after writing a 1.
// 	 -- In CONTINUOUS MODE write a 1 here to start capturing all images
// 	 -- and write a 0 to stop the capture. 
// 	 constant START_CAPTURE_ADDRESS  : integer  := 5;
// 	 -- Signal indicating standby state 
//     --(outside of reset, waiting for flank in start_capture)
// 	 --In SINGLE_SHOT mode it can be used after setting start_capture to 
// 	 --check if writting the image to memory finished
// 	 constant STANDBY_ADDRESS        : integer  := 6;
// 	 --Last buffer indicates which buffer was the last one written (0 or 1)
// 	 constant LAST_BUFFER_ADDRESS    : integer  := 7;
// 	 --Downsampling rate (1=get all image, 2=half of rows and columns so
// 	 --size is reduced by four, 4= one fourth of cols and rows are capture, 
// 	 --so on...)
// 	 constant DOWNSAMPLING_ADDRESS   : integer  := 8;
// 	 --Image counter
// 	 constant IMAGE_COUNTER_ADDRESS  : integer  := 9;

	void *h2p_lw_mode_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_buff0_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 1*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_buff1_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 2*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_cont_double_buff_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 3*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_buffer_select_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 4*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_start_capture_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 5*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_standby_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 6*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_last_buffer_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 7*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_downsampling_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 8*4 ) & ( unsigned long)( HW_REGS_MASK ) );
	void *h2p_lw_image_counter_addr = virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + AVALON_IMG_WRITER_RGBGRAY_BASE + 9*4 ) & ( unsigned long)( HW_REGS_MASK ) );

	*(uint32_t *)h2p_lw_mode_addr = 1;
	printf("Mode: %d\n", *(uint32_t *)h2p_lw_mode_addr);
	*(uint32_t *)h2p_lw_buff0_addr = (uint32_t)(SDRAM_BASE + BUFF0_OFFSET);
	printf("Buff0: %p\n", (void *)(*(uint32_t *)h2p_lw_buff0_addr));
	*(uint32_t *)h2p_lw_buff1_addr = (uint32_t)(SDRAM_BASE + BUFF1_OFFSET);
	printf("Buff1: %p\n", (void *)(*(uint32_t *)h2p_lw_buff1_addr));
	*(uint32_t *)h2p_lw_cont_double_buff_addr = 0;
	printf("Cont double buff: %d\n", *(uint32_t *)h2p_lw_cont_double_buff_addr);
	*(uint32_t *)h2p_lw_buffer_select_addr = 0;
	printf("Buffer select: %d\n", *(uint32_t *)h2p_lw_buffer_select_addr);
	*(uint32_t *)h2p_lw_downsampling_addr = 1;
	printf("Downsampling: %d\n", *(uint32_t *)h2p_lw_downsampling_addr);
	*(uint32_t *)h2p_lw_image_counter_addr = 0;
	printf("Image counter: %d\n", *(uint32_t *)h2p_lw_image_counter_addr);
	*(uint32_t *)h2p_lw_last_buffer_addr = 0;
	printf("Last buffer: %d\n", *(uint32_t *)h2p_lw_last_buffer_addr);
	*(uint32_t *)h2p_lw_start_capture_addr = 1; 
	printf("Start capture: %d\n", *(uint32_t *)h2p_lw_start_capture_addr);

	printf("Standby: %d, Last buffer: %d, Image counter: %d\n", *(uint32_t *)h2p_lw_standby_addr, *(uint32_t *)h2p_lw_last_buffer_addr, *(uint32_t *)h2p_lw_image_counter_addr);
	printf("Buffer0: "); 
	for (int i = 0; i < 100; i++) {
		printf("%08X,", buff0_addr[i]);
	}
	printf("\n");

	// clean up our memory mapping and exit	
	munmap(virtual_base, HW_REGS_SPAN); 
	close( fd );

	return( 0 );
}
