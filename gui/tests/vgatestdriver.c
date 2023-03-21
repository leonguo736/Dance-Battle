#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
//#include <intelfpgaup/video.h>
//#include <intelfpgaup/KEY.h>
//#include <intelfpgaup/SW.h>
//#include <intelfpgaup/audio.h>

#include "video.h"
#include "KEY.h"
#include "SW.h"
#include "audio.h"

#define AUDIO_RATE 8000
#define VOLUME 0x7FFFFFF

#define D5SHARP 622.25
#define F5 698.46

double C_SCALE[8] = {261.63,293.66,329.63,349.23,392.00,440.00,493.88,523.25};

#define PLUM 0x3947 // 7,10,7
#define PLUM_DARK 0x18a3 // 3,5,3
#define PLUM_LIGHT 0x59eb // 11,15,11

#define WIDTH 320
#define HEIGHT 240
#define BORDER_PADDING 10

#define SONGLIST_TOP 20
#define SONGLIST_LEFT 20
#define SONGLIST_WIDTH 50
#define SONGLIST_HEIGHT 25
#define SONGLIST_ELEMENTS 4

#define SONGLIST_COLOR1 0xc638
#define SONGLIST_COLOR2 0xffff
#define SONGLIST_FONTCOLOR 0x18e2

int screen = 0;

double angle = 0;
double lastAngles[2] = {0, 0};
double angleVelocity = 0;

char * songNames[8] = {"Song A", "Song B", "Song C", "Song D", "Song E", "Song F", "Never Gonna Give You Up"};
short songColors[8] = {video_RED, video_ORANGE, video_YELLOW, video_GREEN, video_BLUE, video_MAGENTA, video_PINK, video_GREY};
int songSelection = 0;
int songTop = 0;

void switchScreen(void) {
    video_show();
    screen = (screen + 1) % 2;
}

void sketchCircle(double centerX, double centerY, int radius, short colour) {
    int radiusSquared = radius * radius;
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            if (x * x + y * y < radiusSquared) {
                video_pixel(round(centerX) + x, round(centerY) + y, colour);
            }
        }
    }
}

void drawBackground(void) {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (x < BORDER_PADDING || y < BORDER_PADDING || x > WIDTH - BORDER_PADDING || y > HEIGHT - BORDER_PADDING) {
                video_pixel(x, y, PLUM_DARK);
            } else {
                video_pixel(x, y, PLUM);
            }
        }
    }
    switchScreen();
}

void drawHand() {
    video_line(200, 120, 200 + 80 * cos(lastAngles[screen]), 120 + 80 * sin(lastAngles[screen]), PLUM);
    sketchCircle(200 + 80 * cos(lastAngles[screen]), 120 + 80 * sin(lastAngles[screen]), 5, PLUM);
    video_line(200, 120, 200 + 80 * cos(angle), 120 + 80 * sin(angle), video_BLUE);
    sketchCircle(200 + 80 * cos(angle), 120 + 80 * sin(angle), 5, video_BLUE);
    lastAngles[screen] = angle;
    switchScreen();
}

void drawSongList() {
    for (int i = 0; i < SONGLIST_ELEMENTS; i++) {
        short color = i + songTop == songSelection ? SONGLIST_COLOR2 : SONGLIST_COLOR1;
        video_box(SONGLIST_LEFT, SONGLIST_TOP + SONGLIST_HEIGHT * i, SONGLIST_LEFT + SONGLIST_WIDTH, SONGLIST_TOP + SONGLIST_HEIGHT * (i + 1), color);
        video_box(SONGLIST_LEFT + 5, SONGLIST_TOP + SONGLIST_HEIGHT * i + 5, SONGLIST_LEFT + SONGLIST_WIDTH - 5, SONGLIST_TOP + SONGLIST_HEIGHT * (i + 1) - 5, songColors[i + songTop]);
        //video_text(SONGLIST_LEFT, SONGLIST_TOP + SONGLIST_HEIGHT * i, songNames[i + songTop]);
    }
    switchScreen();
}

void writeAudio(int sample) {
    audio_wait_write();
    audio_write_left(sample);
    audio_write_right(sample);
}

void playTone(double frequency, double duration) {
    int sampleCount = (int)(duration * AUDIO_RATE);
    int sampleArray[sampleCount];
    for (int s = 0; s < sampleCount; s++) {
        sampleArray[s] = (int)(VOLUME * sin(s * M_PI * frequency / AUDIO_RATE));
    }
    for (int s = 0; s < sampleCount; s++) {
        writeAudio(sampleArray[s]);
    }
}

void * vgaThread(void *vargp) {
    while (1) {
        angle += angleVelocity;
        drawHand();
        //drawSongList();
        sleep(1 / 60.0);
    }
}

void * audioThread(void *vargp) {
    for (int tone = 0; tone < 8; tone++) {
        playTone(C_SCALE[tone], 0.5);
    }
    while (1);
}

int main(void) {

    if (!video_open()) return -1;
    if (!SW_open()) return -1;
    if (!KEY_open()) return -1;
    if (!audio_open()) return -1;
    audio_init();
    audio_rate(AUDIO_RATE);

    video_clear();
    video_erase();

    drawBackground();
    drawBackground();

    pthread_t vga_thread_id;
    pthread_create(&vga_thread_id, NULL, vgaThread, NULL);
    
    pthread_t audio_thread_id;
    pthread_create(&audio_thread_id, NULL, audioThread, NULL);

    int swState = 0;
    int keyState = 0;

    while (swState != 0b1111) {

        // Get UI inputs
        SW_read(&swState);
        KEY_read(&keyState);

        // Rotating hand
        angleVelocity = swState / 64.0;

        if (swState == 1) {
            if (songSelection < 7) {
                songSelection++;
                if (songSelection - songTop >= SONGLIST_ELEMENTS) {
                    songTop++;
                }
            }
        } else if (swState == 2) {
            if (songSelection > 0) {
                songSelection--;
                if (songSelection - songTop < 0) {
                    songTop--;
                }
            }
        }
    }

    video_close();
    SW_close();
    KEY_close();
    audio_close();

    return 0;
}
