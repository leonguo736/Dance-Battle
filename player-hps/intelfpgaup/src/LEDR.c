#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/LEDR.h"

/* This file contains wrappers for the red light LEDR character device driver */
int LEDR_FD = -1;

#define LEDR_CHAR_DEV "/dev/IntelFPGAUP/LEDR"

/* Declare buffer for writing data to device driver */
#define BUF_SIZE 32							// reserve some buffer space
char LEDR_buffer[BUF_SIZE];				// buffer for LEDR data

/**
 * Opens the red light LEDR device. 
 * Return: 1 on success, else 0
 */
int LEDR_open( )
{
 	if ((LEDR_FD = open (LEDR_CHAR_DEV, O_RDWR)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s\n", LEDR_CHAR_DEV);
		return 0;
	}
	else
		return 1;
}

/**
 * Sets the red lights LEDR 
 * Parameter data: written to LEDR device
 * Return: void
 */
void LEDR_set(int data)
{
	if (LEDR_FD > 0)
	{
		sprintf (LEDR_buffer, "%03X", data);
		write (LEDR_FD, LEDR_buffer, strlen(LEDR_buffer));
	}
	else
		fprintf (stderr, "Can't write %s: LEDR_FD = %d\n", LEDR_CHAR_DEV, LEDR_FD);
}

/**
 * Closes the LEDR device
 * Return: void
 */
void LEDR_close( )
{
	if (LEDR_FD > 0)
		close (LEDR_FD);
}

