#include "audio.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hps_0.h"
#include "hwlib.h"
#include "regs.h"
#include "socal/hps.h"

/* This file contains wrappers for the audio character device driver */
int audio_FD = -1;

#define audio_CHAR_DEV "/dev/IntelFPGAUP/audio"

#define AUDIO_BYTES 24				// number of bytes to read from audio device
#define BUF_THRESHOLD 96
#define FIFO_REG_OFFSET 4
#define FIFO_OUTPUT_WORD_MASK 0xFFFF0000

/* Declare buffers for reading/writing data to device drivers */
#define BUF_SIZE 128						// large enough for all drivers
char audio_buffer[BUF_SIZE];				// buffer for audio data

volatile uint32_t* FIFO_REG = NULL;

/* 
 * Initializes audio fifospace register
 */
void audio_init_reg(void* virtual_base) {
	FIFO_REG = (uint32_t*)(virtual_base + ((unsigned long) (ALT_LWFPGASLVS_OFST + AUDIO_SUBSYSTEM_AUDIO_BASE + FIFO_REG_OFFSET) &
                    (unsigned long)(HW_REGS_MASK)));
}

/**
 * Opens the digital audio device
 * Return: 1 on success, else 0
 */
int audio_open( )
{
 	if ((audio_FD = open (audio_CHAR_DEV, O_RDWR)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s\n", audio_CHAR_DEV);
		return 0;
	}
	else
		return 1;
}

/**
 * Reads data from the audio device
 * Parameter ldata: pointer for returning the left-channel data
 * Parameter rdata: pointer for returning the right-channel data
 * Return: 1 on success, else 0
 */
int audio_read(int *ldata, int *rdata)
{
  	int bytes, bytes_read;
	bytes_read = 0;
	if (audio_FD > 0)
	{
		while ((bytes = read (audio_FD, audio_buffer, AUDIO_BYTES)) > 0)
			bytes_read += bytes;	// read the port until EOF
		if (bytes_read != AUDIO_BYTES)
		{
			fprintf (stderr, "Error: %d bytes expected from %s, but %d bytes read\n", AUDIO_BYTES, 
				audio_CHAR_DEV, bytes_read);
			return 0;
		}
		if (sscanf (audio_buffer, "%d %d", ldata, rdata) != 2)
		{
			fprintf (stderr, "Can't parse data from %s: %s\n", audio_CHAR_DEV, audio_buffer);
			return 0;
		}
	}
	else
	{		  
		fprintf (stderr, "Can't read %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
		return 0;
	}
	return 1;
}

/**
 * Initializes the digital audio device
 * Return: void
 */
void audio_init( )
{
	if (audio_FD > 0)
		write (audio_FD, "init", 4);		// initialize audio port
	else
		fprintf (stderr, "Can't write %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
}

/**
 * Sets the sampling rate of the digital audio device
 * Parameter data: the sampling rate in samples per second (8000, 32000, or 48000)
 * Return: void
 */
void audio_rate(int data)
{
	if (audio_FD > 0)
	{
		sprintf (audio_buffer, "rate %d", data);
		write (audio_FD, audio_buffer, strlen(audio_buffer));
	}
	else
		fprintf (stderr, "Can't write %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
}

/**
 * Waits for space to be available for writing to the digital audio device
 * Return: void
 */
void audio_wait_write( )
{
	if (audio_FD > 0)
		write (audio_FD, "waitw", 5);		// wait for space in the write FIFOs
	else
		fprintf (stderr, "Can't write %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
}	

/*
 * Check if space is available for writing to the digital audio device
 */

int audio_check_write( )
{
	// printf("audio_check_write | %x\n", *FIFO_REG);
	return *FIFO_REG & FIFO_OUTPUT_WORD_MASK;
}



/**
 * Waits until data is available for reading from the digital audio device
 * Return: void
 */
void audio_wait_read ( )
{
	if (audio_FD > 0)
		write (audio_FD, "waitr", 5);		// wait for data in the read FIFOs
	else
		fprintf (stderr, "Can't write %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
}	

/**
 * Writes data to the left channel of the digital audio device
 * Parameter data: left-channel data
 * Return: void
 */
void audio_write_left(int data)
{
	if (audio_FD > 0)
	{
		sprintf (audio_buffer, "left %d", data);
		write (audio_FD, audio_buffer, strlen(audio_buffer));
	}
	else
		fprintf (stderr, "Can't write %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
}

/**
 * Writes data to the right channel of the digital audio device
 * Parameter data: right-channel data
 * Return: void
 */
void audio_write_right(int data)
{
	if (audio_FD > 0)
	{
		sprintf (audio_buffer, "right %d", data);
		write (audio_FD, audio_buffer, strlen(audio_buffer));
	}
	else
		fprintf (stderr, "Can't write %s: audio_FD = %d\n", audio_CHAR_DEV, audio_FD);
}

/**
 * Closes the digital audio device
 * Return: void
 */
void audio_close( )
{
	if (audio_FD > 0)
		close (audio_FD);
}
