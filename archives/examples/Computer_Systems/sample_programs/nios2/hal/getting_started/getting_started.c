#include "system.h"
#include "altera_avalon_pio_regs.h"

#ifndef LEDS_BASE 
#define LEDS_BASE RED_LEDS_BASE 
#endif

#ifndef SLIDER_SWITCHES_BASE
#define SLIDER_SWITCHES_BASE SWITCHES_BASE
#endif


/*******************************************************************************
 * This program demonstrates use of parallel ports.
 *
 * It performs the following: 
 *  1. displays a rotating pattern on the LEDs
 *  2. if any KEY is pressed, the SW switches are used as the rotating pattern
 ******************************************************************************/

int main(void)
{
    /* The base addresses of devices are listed in the "BSP/system.h" file*/
    
	int LED_bits = 0x0F0F0F0F;					// pattern for LED lights
	int SW_value, KEY_value;
	volatile int delay_count;					// volatile so the C compiler doesn't remove the loop

	while(1)
	{
		SW_value = IORD_ALTERA_AVALON_PIO_DATA(SLIDER_SWITCHES_BASE);	// read the SW slider switch values

		KEY_value = IORD_ALTERA_AVALON_PIO_DATA(PUSHBUTTONS_BASE); 		// read the pushbutton KEY values
        
		if (KEY_value != 0)						// check if any KEY was pressed
		{
			/* set pattern using SW values */
			LED_bits = SW_value | (SW_value << 8) | (SW_value << 16) | (SW_value << 24);
			/* wait for pushbutton KEY release */
			while (IORD_ALTERA_AVALON_PIO_DATA(PUSHBUTTONS_BASE));
		}

		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, LED_bits);			// light up the green LEDs
        
		/* rotate the pattern shown on the LEDs */
		if (LED_bits & 0x80000000)
			LED_bits = (LED_bits << 1) | 1;
		else
			LED_bits = LED_bits << 1;

		for (delay_count = 200000; delay_count != 0; --delay_count);	// delay loop
	}
    return 0;
}
