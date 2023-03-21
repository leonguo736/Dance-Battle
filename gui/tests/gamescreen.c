#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/KEY.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/audio.h>

#include "display.h"
#include "youwantgnomes.h"
#include "font.h"

// Audio

#define AUDIO_RATE 8000
#define VOLUME 0x7FFFFFFF

#define SAMPLES_PER_FRAME 200 // 40 FPS
#define PIXELS_PER_SAMPLE 0.01375 // Pose movement speed
#define POSE_LIFETIME 16000 // In samples

struct Song song;
int started = 0;
int state = 0;

struct GamescreenState {
    int progressBarWidth;
    int earlyPose;
    int latePose;
    int poseXs[MAX_POSES];
};

int poseXs1[MAX_POSES];
int poseXs2[MAX_POSES];

struct GamescreenState gamescreenStates[2] = {
    {0, -1, -1, poseXs1},
    {0, -1, -1, poseXs2}
};

// Audio playing
void writeAudio(int l, int r, int skip) {
    // This function maintains the game speed
    if (!skip) audio_wait_write();
    audio_write_left(l);
    audio_write_right(r);
}

// Use to switch the screen (also switches state)
void switchScreen(void) {
    video_show();
    state = !state;
}

void updateProgressBar() {
    int oldWidth = gamescreenStates[state].progressBarWidth;
    int newWidth = (int)(159 * (double)song.sampleNo / MAX_SAMPLES);
    gamescreenStates[state].progressBarWidth = newWidth;

    video_box(81 + oldWidth, 211, 81 + newWidth, 224, COLOR_PROGRESSFILL);
}

int getPoseX(struct ScreenPose sp) {
    int x = (int)(50 + PIXELS_PER_SAMPLE * (sp.sample - song.sampleNo + song.sampleOffset));
    if (x >= 50 && x <= 270) {
        return x;
    }
    return 0;
}

void drawPose(int x, int hx, int hy, int mx, int my, int erase) {
    drawVLine();
    video_line(x, 120, x + hx, 120 + hy, erase ? COLOR_BG : COLOR_HHAND);
    video_line(x, 120, x + mx, 120 + my, erase ? COLOR_BG : COLOR_MHAND);
    // video_pixel(x, 50, erase ? COLOR_HLINE : COLOR_DOT);
    // video_pixel(x, 190, erase ? COLOR_HLINE : COLOR_DOT);
    video_line(x, 51, x, 189, erase ? COLOR_BG : COLOR_VLINE);
}

void updatePoses(void) {
    int oldEarly = gamescreenStates[state].earlyPose;
    int oldLate = gamescreenStates[state].latePose;
    if (oldEarly != -1) {
        for (int p = oldEarly; p <= oldLate; p++) {
            int oldX = gamescreenStates[state].poseXs[p];
            struct ScreenPose sp = song.screenPoses[p];
            drawPose(oldX, sp.hx, sp.hy, sp.mx, sp.my, 1);
        }
    }

    // Advance pose range if necessary
    if (song.earlyPose == -1) {
        if (song.sampleNo >= song.screenPoses[0].sample - POSE_LIFETIME) {
            song.earlyPose = 0;
            song.latePose = 0;
        }
    } else {
        if (song.latePose < song.numPoses && song.sampleNo >= song.screenPoses[song.latePose + 1].sample - POSE_LIFETIME) {
            song.latePose++;
        }
        if (song.earlyPose < song.numPoses && song.sampleNo >= song.screenPoses[song.earlyPose].sample) {
            song.earlyPose++;
        }
    }

    gamescreenStates[state].earlyPose = song.earlyPose;
    gamescreenStates[state].latePose = song.latePose;

    // Advance pose x
    if (song.earlyPose != -1) {
        for (int p = song.earlyPose; p <= song.latePose; p++) {
            struct ScreenPose sp = song.screenPoses[p];
            int x = getPoseX(sp);
            sp.x = x;
            gamescreenStates[state].poseXs[p] = x;
            if (x != 0) drawPose(x, sp.hx, sp.hy, sp.mx, sp.my, 0);
        }
    }
}

void drawLetter(int ascii, int tlx, int tly, short color) {
    for (int x = tlx; x < tlx + 7; x++) {
        for (int y = tly; y < tly + 9; y++) {
            if (font[ascii][x - tlx][y - tly]) {
                video_pixel(x, y, color);
            }
        }
    }
}

void drawWord(char word[], int tlx, int tly, int kerning, short color) {
    int xOff = 0;
    for (int c = 0; c < strlen(word); c++) {
        drawLetter(word[c], tlx + xOff, tly, color);
        xOff += 7 + kerning;
    }
}

void updateDisplay() {
    updateProgressBar();
    updatePoses();
    drawLetter(97, 10, 10, COLOR_PROGRESSFILL);
    drawLetter(98, 18, 10, COLOR_DOT);
    drawLetter(99, 26, 10, COLOR_HLINE);
    drawLetter(100, 34, 10, COLOR_PROGRESSBORDER);
    drawWord("welcome to dance battle", 70, 25, 1, COLOR_PROGRESSFILL);
    switchScreen();
}

void * guiThread(void *vargp) {
    audio_init();
    audio_rate(AUDIO_RATE);

    video_clear();
    video_erase();
    drawBackground();

    // Init song data

    int numPoses = 8;
    struct Pose poses[MAX_POSES] = {
        {16, 0, 0.1},
        {18, 0.2, 0.3},
        {20, 0.4, 0.6},
        {22, 0.5, 0.7}
    };

    // double nothingL[480000];
    // double nothingR[480000];

    song = initSong( AUDIO_RATE / (174.0 / 60), 0, (double *)youwantgnomes_L, (double *)youwantgnomes_R, numPoses, poses);

    while (!started);

    int frame = 0;

    // Loop

    for (song.sampleNo; song.sampleNo < MAX_SAMPLES; song.sampleNo++) {
        writeAudio(VOLUME * song.samplesL[song.sampleNo], VOLUME * song.samplesR[song.sampleNo], song.sampleNo % SAMPLES_PER_FRAME == 0);
        //writeAudio(0, 0);

        // Update display
        if (song.sampleNo % SAMPLES_PER_FRAME == 0) {
            updateDisplay();
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