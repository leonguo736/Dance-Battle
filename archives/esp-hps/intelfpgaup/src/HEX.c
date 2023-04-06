#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/HEX.h"

/* This file contains wrappers for the 7-segment display HEX character device driver */
int HEX_FD = -1;

#define HEX_CHAR_DEV "/dev/IntelFPGAUP/HEX"

/* Declare buffer for writing data to device driver */
#define BUF_SIZE 32							// large enough for all drivers
char HEX_buffer[BUF_SIZE];					// buffer for HEX data

/**
 * Opens the 7-segment HEX device.
 * Return: 1 on success, else 0
 */
int HEX_open( )
{
 	if ((HEX_FD = open (HEX_CHAR_DEV, O_RDWR)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s\n", HEX_CHAR_DEV);
		return 0;
	}
	else
		return 1;
}

/**
 * Sets the 7-segment HEX displays to decimal digits from 0-9
 * Parameter data: the data to be displayed as a 6-digit decimal number. The upper two
 * 	digits will be displayed on HEX5-4, and the lower four digits on HEX3-0
 * Return: void
 */
void HEX_set(int data)
{
	if (HEX_FD > 0)
	{
		sprintf (HEX_buffer, "%06d", data);
		write (HEX_FD, HEX_buffer, strlen(HEX_buffer));
	}
	else
		fprintf (stderr, "Can't write %s: HEX_FD = %d\n", HEX_CHAR_DEV, HEX_FD);
}

/**
 * Sets the 7-segment HEX displays to any (raw) value
 * Parameter data_h: the lowest 16 bits are displayed on HEX5-4
 * Parameter data_l: a 32-bit value that is displayed on HEX3-0
 * Return: void
 */
void HEX_raw(int data_h, int data_l)
{
	if (HEX_FD > 0)
	{
		sprintf (HEX_buffer, "-r %04X %08X", data_h, data_l);
		write (HEX_FD, HEX_buffer, strlen(HEX_buffer));
	}
	else
		fprintf (stderr, "Can't write %s: HEX_FD = %d\n", HEX_CHAR_DEV, HEX_FD);
}

/**
 * Closes the HEX device
 * Return: void
 */
void HEX_close( )
{
	if (HEX_FD > 0)
		close (HEX_FD);
}
