#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#ifdef GCC
#include "KEY.h"
#include "SW.H"
#include "audio.h"
#include "video.h"
#else
#include <intelfpgaup/KEY.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/audio.h>
#include <intelfpgaup/video.h>
#endif

#include "fonts.h"
#include "song.h"
#include "sampledata.h"

int started = 0;
int screen = 0; // 0 for init, 1 for lobby, 2 for game, 3 for results
int buffer = 0; // 0 or 1

struct InitScreenState {
    double dotPositions[INIT_NUM_DOTS];
};

struct InitScreenState initScreenStates[2] = {
    { {0.0} },
    { {0.0} }
};

struct LobbyScreenState {
    int p1Connected;
    int p2Connected;
    int mode;
    int songId;
};

struct LobbyScreenState lobbyScreenStates[2] = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
};

struct GameScreenState {
    int pBarWidth;
    int earlyPose;
    int latePose;
    int poseXs[MAX_POSES];
};

int poseXs1[MAX_POSES];
int poseXs2[MAX_POSES];
struct GameScreenState gameScreenStates[2] = {
    { 0, -1, -1, poseXs1 },
    { 0, -1, -1, poseXs2 }
};

struct ResultsScreenState {

};

struct ResultsScreenState resultsScreenStates[2] = {
    { },
    { }
};

void writeAudio(int l, int r, int skip) {
    if (!skip) audio_wait_write();
    audio_write_left(l);
    audio_write_right(r);
}

void switchBuffer(void) {
    video_show();
    buffer = !buffer;
}

void initGraphics(int screen) {
    for (int i = 0; i < 2; i++) { // Put the same thing on both buffers
        if (screen == 0) { // Init
            drawBackground();
            drawString(italicFont, "dance", 50, 30, COLOR_INIT_DANCE, 3, 2);
            drawString(italicFont, "battle", 218, 60, COLOR_INIT_BATTLE, 2, 2);
            drawString(basicFont, "waiting for server connection", 30, 200, COLOR_INIT_STATUS, 1, 1);
        } else if (screen == 1) { // Lobby
            drawBackground();
        } else if (screen == 2) { // Game
            drawBackground();

            // Straight lines
            video_line(BORDER, 50, WIDTH - BORDER, 50, COLOR_GAME_HLINE);
            video_line(BORDER, HEIGHT - 50, WIDTH - BORDER, HEIGHT - 50, COLOR_GAME_HLINE);
            video_line(50, BORDER, 50, HEIGHT - BORDER, COLOR_GAME_VLINE);
            video_line(WIDTH - BORDER, BORDER, WIDTH - BORDER, HEIGHT - BORDER, COLOR_GAME_VLINE);

            drawPBarOutline();
        }
        switchBuffer();
    }
}

void switchScreen(int s) {
    printf("Screen: %d\n", s);
    screen = s;
    initGraphics(s);
}

void * outputThread(void *vargp) {

    // Initialization
    audio_init();
    audio_rate(AUDIO_RATE);
    switchScreen(0);

    // Loop

    while (1) {
        if (screen == 0) {

        }
    }
}

int main(void) {

    if (!audio_open()) return 1;
    if (!video_open()) return 1;
    if (!SW_open()) return 1;
    if (!KEY_open()) return 1;

    pthread_t outputThread_id;
    pthread_create(&outputThread_id, NULL, outputThread, NULL);

    int swState = 0;
    int keyState = 0;

    while (keyState != 8) {

        SW_read(&swState);
        KEY_read(&keyState);
        
        if (keyState == 1) switchScreen(0);
        else if (keyState == 2) switchScreen(1);
        else if (keyState == 4) switchScreen(2);
    }

    audio_close();
    video_close();
    SW_close();
    KEY_close();

    return 0;
}
