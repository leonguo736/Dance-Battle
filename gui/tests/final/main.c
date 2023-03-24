#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
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

#include "fontdata.h"
#include "song.h"
#include "sampledata.h"

int screen = 0; // 0 for init, 1 for lobby, 2 for game, 3 for results
int switchScreenLock = 0;
int graphicsUpdateLock = 0;
int buffer = 0; // 0 or 1

int titleSlideDone = 0;
int statusSlideDone = 0;
const char * init_names[] = {"alex", "bell", "kery", "leon"};
int init_name_speeds[] = {4, 4, -4, -4};
int init_name_offsets[] = {0, 0, 40, 40};
short color_init_names[] = {0xe4a, 0xd19, 0xc641, 0x7859};

#define FPS 40
#define SAMPLES_PER_FRAME 200 // Recalculate if you change the audio rate or frame rate

struct InitScreenState {
    double danceX;
    double battleX;
    double statusY;
    int namePositions[4][4][2];
};

struct InitScreenState initScreenStates[2];

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

struct GameScreenState gameScreenStates[2] = {
    { 0, -1, -1, { 0 } },
    { 0, -1, -1, { 0 } }
};

struct ResultsScreenState {

};

struct ResultsScreenState resultsScreenStates[2] = {
    { },
    { }
};

void resetInitState(void) {
    for (int i = 0; i < 2; i++) {
        struct InitScreenState state = {
            240.0,
            -100.0,
            240.0,
            {
                {{0},{8},{16},{24}},
                {{160},{168},{176},{184}},
                {{80},{88},{96},{104}},
                {{240},{248},{256},{264}}
            }
        };
        initScreenStates[i] = state;
    }
}

void writeAudio(int l, int r) {
    audio_write_left(l);
    audio_write_right(r);
}

void switchBuffer(void) {
    video_show();
    buffer = !buffer;
}

void initGraphics(int s) {
    while (graphicsUpdateLock);
    switchScreenLock = 1;
    screen = s;
    for (int i = 0; i < 2; i++) { // Put the same thing on both buffers
        if (s == 0) { // Init
            titleSlideDone = 0;
            statusSlideDone = 0;
            resetInitState();
            drawBackground();
            video_box(BORDER + 1, 110, WIDTH - BORDER - 1, 180, COLOR_INIT_TUBE);
        } else if (s == 1) { // Lobby
            drawBackground();
        } else if (s == 2) { // Game
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
    switchScreenLock = 0;
}

double lerp(double current, double target, double factor) {
    return current + (target - current) * factor;
}

void updateGraphics(void) {
    graphicsUpdateLock = 1;
    if (screen == 0) { // Init
        struct InitScreenState * prevState = &(initScreenStates[buffer]);
        struct InitScreenState * otherState = &(initScreenStates[(buffer+1)%2]);

        if (!titleSlideDone) {
            int dx = (int)round(prevState->danceX);
            int bx = (int)round(prevState->battleX);

            drawString(italicFont, "dance", dx, INIT_DANCE_Y, COLOR_BG, INIT_DANCE_SCALE, INIT_DANCE_KERNING);
            drawString(italicFont, "battle", bx, INIT_BATTLE_Y, COLOR_BG, INIT_BATTLE_SCALE, INIT_BATTLE_KERNING);

            drawHLine(INIT_DANCE_Y + 35, dx + 147, dx + 230, COLOR_BG);
            drawHLine(INIT_DANCE_Y + 30, dx + 147, dx + 180, COLOR_BG);

            drawHLine(INIT_BATTLE_Y, bx - 140, bx - 4, COLOR_BG);
            drawHLine(INIT_BATTLE_Y + 5, bx - 55, bx - 4, COLOR_BG);
            drawHLine(INIT_BATTLE_Y + 10, bx - 25, bx - 4, COLOR_BG);

            prevState->danceX = lerp(prevState->danceX, INIT_DANCE_X, INIT_TITLE_LERP);
            prevState->battleX = lerp(prevState->battleX, INIT_BATTLE_X, INIT_TITLE_LERP);
            dx = (int)round(prevState->danceX);
            bx = (int)round(prevState->battleX);

            drawString(italicFont, "dance", dx, INIT_DANCE_Y, COLOR_INIT_DANCE, INIT_DANCE_SCALE, INIT_DANCE_KERNING);
            drawString(italicFont, "battle", bx, INIT_BATTLE_Y, COLOR_INIT_BATTLE, INIT_BATTLE_SCALE, INIT_BATTLE_KERNING);

            drawHLine(INIT_DANCE_Y + 35, dx + 147, dx + 230, COLOR_INIT_HLINE);
            drawHLine(INIT_DANCE_Y + 30, dx + 147, dx + 180, COLOR_INIT_HLINE);

            drawHLine(INIT_BATTLE_Y, bx - 140, bx - 4, COLOR_INIT_HLINE);
            drawHLine(INIT_BATTLE_Y + 5, bx - 55, bx - 4, COLOR_INIT_HLINE);
            drawHLine(INIT_BATTLE_Y + 10, bx - 25, bx - 4, COLOR_INIT_HLINE);

            if (dx == INIT_DANCE_X && bx == INIT_BATTLE_X && (int)round(otherState->battleX) == INIT_BATTLE_X) {
                titleSlideDone = 1;
            }
        } else if (!statusSlideDone) {
            int sy = (int)round(prevState->statusY);
            drawString(basicFont, "waiting for server connection", INIT_STATUS_X, sy, COLOR_BG, INIT_STATUS_SCALE, INIT_STATUS_KERNING);
            prevState->statusY = lerp(prevState->statusY, INIT_STATUS_Y, INIT_TITLE_LERP);
            sy = (int)round(prevState->statusY);
            drawString(basicFont, "waiting for server connection", INIT_STATUS_X, sy, COLOR_INIT_STATUS, INIT_STATUS_SCALE, INIT_STATUS_KERNING);

            if (sy == INIT_STATUS_Y && (int)round(otherState->statusY) == INIT_STATUS_Y) {
                statusSlideDone = 1;
            }
        } else {
            for (int n = 0; n < 4; n++) {
                for (int c = 0; c < 4; c++) {
                    if ((prevState->namePositions)[n][c][1] != 0) {
                        drawChar(italicFont, init_names[n][c], (prevState->namePositions)[n][c][0], (prevState->namePositions)[n][c][1], COLOR_INIT_TUBE, 1);
                    }
                }
            }
            for (int n = 0; n < 4; n++) {
                for (int c = 0; c < 4; c++) {
                    int newX = (otherState->namePositions)[n][c][0] + init_name_speeds[n];
                    if (newX >= WIDTH) newX -= WIDTH;
                    if (newX < 0) newX += WIDTH;
                    int newY = sin(INIT_NAME_FREQUENCY * (newX + init_name_offsets[n])) * INIT_NAME_AMPLITUDE + INIT_NAME_Y;
                    drawChar(italicFont, init_names[n][c], newX, newY, color_init_names[n], 1);
                    (prevState->namePositions)[n][c][0] = newX;
                    (prevState->namePositions)[n][c][1] = newY;
                }
            }
        }
    } else if (screen == 1) { // Lobby

    } else if (screen == 2) { // Game

    }

    switchBuffer();
    graphicsUpdateLock = 0;
}

void * outputThread(void *vargp) {

    // Initialization

    audio_init();
    audio_rate(AUDIO_RATE);
    initGraphics(0);

    // Loop

    int s = 0;

    while (1) {
        writeAudio(0, 0);
        s = (s + 1) % SAMPLES_PER_FRAME;
        if (s == 0) {
            while (switchScreenLock);
            updateGraphics();
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
        
        if (keyState == 1) initGraphics(0);
        else if (keyState == 2) initGraphics(1);
        else if (keyState == 4) initGraphics(2);
    }

    audio_close();
    video_close();
    SW_close();
    KEY_close();

    return 0;
}
