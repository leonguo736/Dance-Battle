#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
//#include <intelfpgaup/SW.h>
//#include <intelfpgaup/audio.h>

#include "SW.h"
#include "audio.h"

#define AUDIO_RATE 48000
#define VOLUME 0x7FFFFFF

double C_SCALE[8] = {261.63,293.66,329.63,349.23,392.00,440.00,493.88,523.25};

void writeAudio(int l, int r) {
    audio_wait_write();
    audio_write_left(l);
    audio_write_right(r);
}

void playTone(double frequency, double duration) {
    int sampleCount = (int)(duration * AUDIO_RATE);
    int sampleArray[sampleCount];
    for (int s = 0; s < sampleCount; s++) {
        sampleArray[s] = (int)(VOLUME * sin(s * M_PI * frequency / AUDIO_RATE));
    }
    for (int s = 0; s < sampleCount; s++) {
        writeAudio(sampleArray[s], sampleArray[s]);
    }
}

int main(void) {

    if (!audio_open()) return 1;
    audio_init();
    audio_rate(AUDIO_RATE);

    // for (int i = 0; i < NUM_SAMPLES; i++) {
    //     writeAudio(frogsarefriends_L[i], frogsarefriends_R[i]);
    // }

    for (int tone = 0; tone < 8; tone++) {
        playTone(C_SCALE[tone], 0.25);
        printf("Played\n");
    }

    audio_close();

    return 0;
}
