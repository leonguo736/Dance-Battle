#include "address_map_nios2.h"

#define CAMERA_IN_ENABLE 0x00000004

/*******************************************************************************
 * This program demonstrates use of camera and edge detection in the computer.
 *
 * It performs the following: 
 * 	1. enables the camera
 * 	2. changes between regular video and edge detection based upon SW[0]
 * 	3. when a key is pressed takes a picture: ie. captures a current frame and stops the video in
 ******************************************************************************/
int main(void)
{
	volatile int * camera_in_ptr			= (int *) CAMERA_IN_BASE;
	volatile int * edge_detection_in_ptr	= (int *) CAMERA_EDGE_DETECT_CTRL_BASE;
	volatile int * KEY_ptr					= (int *) KEY_BASE;
	volatile int * SW_ptr					= (int *) SW_BASE;
	
	int camera_in		= CAMERA_IN_ENABLE;
   *(camera_in_ptr + 3)	= camera_in;		// Enable the camera input
	*(KEY_ptr + 3)		= 0xF;				// Make sure the edge capture register is cleared

	while(1)
	{
		int keys = *(KEY_ptr + 3);			// Read edge capture register
		if (keys != 0x0) {
			camera_in ^= CAMERA_IN_ENABLE;	// Toggle between picture and video
		   *(camera_in_ptr + 3)	= camera_in;		
			*(KEY_ptr + 3)		= keys;		// Clear edge capture register
		}

		int switches = *SW_ptr;				// Toggle between camera and edge detection
		*(edge_detection_in_ptr) = (switches & 0x1);
	}
}
