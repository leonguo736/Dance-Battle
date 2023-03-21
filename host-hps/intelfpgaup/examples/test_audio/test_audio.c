#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <intelfpgaup/audio.h>

// The frequencies of the notes (C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B, C)
#define NUM_NOTES 13
static float note_freqs[NUM_NOTES] = {261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000, 466.164, 493.883, 523.251};

#define NOTE_DURATION 1000	// milliseconds
#define PI2 6.28318531

int main()
{
	// Locals
	int note_index, i, volume, num_samples, sampling_rate;
	double rads_per_sample;
	time_t start, end;
	double delta;

	// Open audio device and initialize it
	if (!audio_open ( ))
	{
		return (-1);
	}
	audio_init ( );
	sampling_rate = 8000;
	audio_rate (sampling_rate);

	// Volume = MAX_VOLUME/8.
	volume = MAX_VOLUME >> 3;

	// Calculate number of samples per note based on the duration and sampling rate.
	num_samples = NOTE_DURATION * sampling_rate / 1000;

	// Play each note.
	for(note_index = 0; note_index < NUM_NOTES; note_index++)
	{
		// Calculate radians per sample for the current note.
		rads_per_sample = PI2 * note_freqs[note_index] / sampling_rate;

		start = clock();
		// Play single note sinusoid.
		for(i = 0; i < num_samples; i++)
		{
			// Wait for space in the write fifo.
			audio_wait_write ( );
			
			// Write the sample to the left and right audio output.
			audio_write_right ((int)(volume * sin (i * rads_per_sample)));
			audio_write_left ((int)(volume * sin (i * rads_per_sample)));		
		}
		end = clock();
		delta = (double) ((end - start)) / (double) CLOCKS_PER_SEC;
  		printf ("Average time between samples: %.1f usec\n", delta / (double) num_samples * 
			1000000.0);
	}

	// Close audio device.
	audio_close ( );

	return 0;
}

