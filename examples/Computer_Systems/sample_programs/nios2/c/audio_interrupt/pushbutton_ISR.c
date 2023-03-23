#include "address_map_nios2.h"

extern volatile int buffer_index;

/*******************************************************************************
 * Pushbutton - Interrupt Service Routine
 *
 * This ISR checks which KEY has been pressed. If KEY0, then it enables audio-in
 * interrupts (recording). If KEY1, it enables audio-out interrupts (playback).
 ******************************************************************************/
void pushbutton_ISR( void )
{
	volatile int * KEY_ptr = (int *) KEY_BASE;
	volatile int * audio_ptr = (int *) AUDIO_BASE;	// audio port address
	
	int KEY_value;

	KEY_value = *(KEY_ptr + 3);			// read the pushbutton interrupt register
	*(KEY_ptr + 3) = KEY_value; 		// Clear the interrupt

	if (KEY_value == 0x1)				// check KEY0
	{
		// reset the buffer index for recording
		buffer_index = 0;
		// clear audio-in FIFO
		*(audio_ptr) = 0x4;
		// turn off clear, and enable audio-in interrupts
		*(audio_ptr) = 0x1;
	}
	else if (KEY_value == 0x2)			// check KEY1
	{
		// reset counter to start playback
		buffer_index = 0;
		// clear audio-out FIFO
		*(audio_ptr) = 0x8;
		// turn off clear, and enable audio-out interrupts
		*(audio_ptr) = 0x2;
	}
	return;
}
