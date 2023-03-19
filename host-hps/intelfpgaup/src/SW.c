#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/SW.h"

/* This file contain wrappers for the slider switch SW character device driver */
int SW_FD = -1;

#define SW_CHAR_DEV "/dev/IntelFPGAUP/SW"
#define SW_BYTES 4					// number of bytes to read from SW device

#define BUF_SIZE 32					// reserve some buffer space
char SW_buffer[BUF_SIZE];			// buffer for SW data

/**
 * Opens the slide switch SW device. 
 * Return: 1 on success, else 0
 */
int SW_open( )
{
 	if ((SW_FD = open (SW_CHAR_DEV, O_RDWR)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s\n", SW_CHAR_DEV);
		return 0;
	}
	else
		return 1;
}

/**
 * Reads the slide switch SW device
 * Parameter data: pointer for returning data
 * Return: 1 on success, else 0
 */
int SW_read(int *data)
{
  	int bytes, bytes_read;
	bytes_read = 0;
	if (SW_FD > 0)
	{		 
		while ((bytes = read (SW_FD, SW_buffer, SW_BYTES)) > 0)
			bytes_read += bytes;	// read the port until EOF
		if (bytes_read != SW_BYTES)
		{
			fprintf (stderr, "Error: %d bytes expected from %s, but %d bytes read\n", SW_BYTES, 
				SW_CHAR_DEV, bytes_read);
			return 0;
		}
		if (sscanf (SW_buffer, "%X", data) != 1)
		{
			fprintf (stderr, "Can't parse data from %s: %s\n", SW_CHAR_DEV, SW_buffer);
			return 0;
		}
	}
	else
	{		  
		fprintf (stderr, "Can't read %s: SW_FD = %d\n", SW_CHAR_DEV, SW_FD);
		return 0;
	}
	return 1;
}

/**
 * Closes the SW device
 * Return: void
 */
void SW_close( )
{
	if (SW_FD > 0)
		close (SW_FD);
}
