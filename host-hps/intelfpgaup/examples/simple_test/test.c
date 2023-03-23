#include <stdio.h>
#include <stdlib.h>
#include <intelfpgaup/LEDR.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/KEY.h>

//
// Assuming the drivers have been loaded and the library has been installed
// compile using this command: 
//								gcc -Wall -o test main.c -lintelfpgaup
//

int main()
{
	int swval, keyval;

	// Open devices.
	LEDR_open ( );
	SW_open ( );
	KEY_open ( );

	// Reads the switch and key values.
	SW_read (&swval);
	KEY_read (&keyval);

	// Write the switch values to the red LEDs
	LEDR_set (swval);

	// Print the key edge capture values.
	printf("keyval: %d\n", (int)keyval);

	// Close devices.
	LEDR_close ( );
	SW_close ( );
	KEY_close ( );

	return 0;
}

