#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#include "video.h"
#include "KEY.h"
#include "SW.h"
#include "audio.h"
//#include <intelfpgaup/video.h>
//#include <intelfpgaup/KEY.h>
//#include <intelfpgaup/SW.h>
//#include <intelfpgaup/audio.h>

#include "youwantgnomes.h"

// Audio

#define AUDIO_RATE 8000
#define VOLUME 0x7FFFFFF
#define NUM_SAMPLES 480000

// Dimensions

#define WIDTH 320
#define HEIGHT 240

#define BORDER 5

#define POSE_PIXELS_PER_SAMPLE 0.01375

// Colors

#define COLOR_BG 0x2804
#define COLOR_BORDER 0x1802

#define COLOR_HLINE 0xffd8
#define COLOR_VLINE 0x50a7
#define COLOR_DOT 0xefa9
#define COLOR_HHAND 0xee3
#define COLOR_MHAND 0xb3b

#define COLOR_PROGRESSBORDER 0xb194
#define COLOR_PROGRESSFILL 0xfab4

// Structs

struct Pose {
    double beat;   // Song time (beats)
    double shx, shy, mhx, mhy; // Hour hand and minute hand positions
};

struct Song {
    double spb;    // Samples per beat
    double offset; // Samples of offset before the first beat
    struct Pose poses[50];  // Array of Pose information
};
struct Song song;

struct GamescreenState {
    double songProgress;
};

int modifyingScreen = 0;

double songDelay = 4.0;
double songDelayTimer = 0;
int sampleCounter;
struct Song mySong;
int samplesPerFrame = 200; // 40 frames per second

int started = 0;
int state = 0;
struct GamescreenState gamescreenStates[2];

// Audio playing tool
void writeAudio(int l, int r) {
    // This function maintains the game speed
    audio_wait_write();
    audio_write_left(l);
    audio_write_right(r);
}

// DISPLAY TOOLS (for drawing each element)

// Use to switch the screen (also switches state)
void switchScreen(void) {
    video_show();
    modifyingScreen = !modifyingScreen;
}

// Draws the vertical line
void drawVLine(void) {
    video_line(50, BORDER, 50, HEIGHT - BORDER, COLOR_VLINE);
}

// Draws the horizontal lines
void drawHLines(void) {
    video_line(BORDER, 50, WIDTH - BORDER, 50, COLOR_HLINE);
    video_line(BORDER, HEIGHT - 50, WIDTH - BORDER, HEIGHT - 50, COLOR_HLINE);
}

// Draws the outline of the progress bar
void drawProgressOutline(void) {
    video_line(80, 210, 240, 210, COLOR_PROGRESSBORDER);
    video_line(80, 225, 240, 225, COLOR_PROGRESSBORDER);
    video_line(80, 210, 80, 225, COLOR_PROGRESSBORDER);
    video_line(240, 210, 240, 225, COLOR_PROGRESSBORDER);
}

// Places the background on both buffers
void drawBackground(void) {
    for (int s = 0; s < 2; s++) {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                if (x < BORDER || y < BORDER || x > WIDTH - BORDER || y > HEIGHT - BORDER) {
                    video_pixel(x, y, COLOR_BORDER);
                } else {
                    video_pixel(x, y, COLOR_BG);
                }
            }
        }

        drawVLine();
        drawHLines();
        drawProgressOutline();

        video_show();
    }
}

void updateProgressBar(double newProgress) {
    double oldProgress = gamescreenStates[modifyingScreen].songProgress;
    gamescreenStates[modifyingScreen].songProgress = newProgress;

    int x1 = (int)(81 + 158 * oldProgress);
    int x2 = (int)(81 + 158 * newProgress);
    video_box(x1, 211, x2, 224, COLOR_PROGRESSFILL);
}

int getPoseX(int p) {
    struct Pose pose = song.poses[p];
    int x = (int)(50 + POSE_PIXELS_PER_SAMPLE * (pose.beat * song.spb - sampleCounter + song.offset));
    if (x >= 50 && x <= 270) {
        return x;
    }
    return 0;
}

void updatePoses(void) {
    for (int p = 0; p < 50; p++) {

    }
}

void updateDisplay(double newProgress) {
    updateProgressBar(newProgress);
    switchScreen();
}

void * guiThread(void *vargp) {
    audio_init();
    audio_rate(AUDIO_RATE);

    video_clear();
    video_erase();
    drawBackground();

    while (!started);

    int frame = 0;

    for (sampleCounter = 0; sampleCounter < NUM_SAMPLES; sampleCounter++) {
        writeAudio(VOLUME * youwantgnomes_L[sampleCounter], VOLUME * youwantgnomes_R[sampleCounter]);
        //writeAudio(0, 0);

        // Update display at equal intervals in the song
        if (sampleCounter % samplesPerFrame == 0) {
            double newProgress = (double)sampleCounter / NUM_SAMPLES;
            updateDisplay(newProgress);
            frame++;
        }
    }

    while (1);
}

int main(void) {

    if (!audio_open()) return 1;
    if (!video_open()) return 1;
    if (!SW_open()) return 1;
    if (!KEY_open()) return 1;

    // Song data
    song.spb = 8000 / 2.9;
    song.offset = 0;

    pthread_t gui_thread_id;
    pthread_create(&gui_thread_id, NULL, guiThread, NULL);

    int swState = 0;
    int keyState = 0;

    while (swState != 0b1000000000) {

        SW_read(&swState);
        KEY_read(&keyState);

        if (keyState == 1 && !started) {
            started = 1;
        }
    }

    audio_close();
    video_close();
    SW_close();
    KEY_close();

    return 0;
}
