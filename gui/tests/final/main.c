#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/KEY.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/audio.h>

#include "song.h"
#include "fonts.h"
#include "sampledata.h"

int screen = 0; // 0 for init, 1 for lobby, 2 for game, 3 for results
int vgaState = 0; // 0 or 1

struct InitScreenState {
    double loadWheelAngle;
};

struct InitScreenState initScreenStates[2] = {
    { 0.0 },
    { 0.0 }
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
struct GamescreenState gameScreenStates[2] = {
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

void switchScreen(void) {
    video_show();
    vgaState = !vgaState;
}

void * outputThread(void *vargp) {
    audio_init();
    audio_rate(AUDIO_RATE);

    video_clear();
    video_erase();
    drawBackground();
    switchScreen();
    video_clear();
    video_erase();
    drawBackground();
    switchScreen();

    while (!started);
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