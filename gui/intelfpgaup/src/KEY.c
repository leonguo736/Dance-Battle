#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/KEY.h"

/* This file contains wrappers for the pushbutton KEY character device driver */
int KEY_FD = -1;

#define KEY_CHAR_DEV "/dev/IntelFPGAUP/KEY"
#define KEY_BYTES 2					// number of bytes to read from KEY device

#define BUF_SIZE 32					// reserve some buffer space
char KEY_buffer[BUF_SIZE];			// buffer for KEY data

/**
 * Opens the pushbutton KEY device
 * Return: 1 on success, else 0
 */
int KEY_open( )
{
 	if ((KEY_FD = open (KEY_CHAR_DEV, O_RDWR)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s\n", KEY_CHAR_DEV);
		return 0;
	}
	else
		return 1;
}

/**
 * Reads the pushbutton KEY device
 * Parameter data: pointer for returning data
 * Return: 1 on success, else 0
 */
int KEY_read(int *data)
{
	int bytes, bytes_read;
	bytes_read = 0;
	if (KEY_FD > 0)
	{
		while ((bytes = read (KEY_FD, KEY_buffer, KEY_BYTES)) > 0)
			bytes_read += bytes;	// read the port until EOF
		if (bytes_read != KEY_BYTES)
		{
			fprintf(stderr, "Error: %d bytes expected from %s, but %d bytes read\n", KEY_BYTES, 
				KEY_CHAR_DEV, bytes_read);
			return 0;
		}
		if (sscanf (KEY_buffer, "%X", data) != 1)
		{
			fprintf (stderr, "Can't parse data from %s: %s\n", KEY_CHAR_DEV, KEY_buffer);
			return 0;
		}
	}
	else
	{		  
		fprintf (stderr, "Can't read %s: KEY_FD = %d\n", KEY_CHAR_DEV, KEY_FD);
		return 0;
	}
	return 1;
}

/**
 * Closes the KEY device
 * Return: void
 */
void KEY_close( )
{
	if (KEY_FD > 0)
		close (KEY_FD);
}
