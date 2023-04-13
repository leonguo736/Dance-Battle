#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include <stdlib.h>

#include "song.h"
#include "pose.h"
#include "vgatools.h"

// Screen states

struct InitScreenState {
    double danceX;
    double battleX;
    double statusY;
    int namePositions[4][4][2];
};

struct LobbyScreenState {
    int p1Connected;
    int p2Connected;
    int mode;
    int songId;
};

struct GameScreenState {
    int pressedStart;
    int threeDone;
    int twoDone;
    int oneDone;
    int goDone;
    int score;
    int score2;
    double messageKerning;
    double messageKerning2;
    int goodVibes;
    int goodVibes2;

    int pBarWidth;
    int earlyPoseA;
    int earlyPoseD;
    int latePoseA;
    int latePoseD;
    int poseAXs[MAX_POSES];
    int poseDXs[MAX_POSES];
};

struct ResultsScreenState {
    int timer;
    double oldGameoverY;
    double oldContinueY;
    int waveYs[18];
};

// Overall states

struct InitState {
    int titleSlideDone;
    int statusSlideDone;
    struct InitScreenState initScreenStates[2];
};

struct LobbyState {
    int p1Connected;
    int p2Connected;
    int mode;
    int songId;
    struct LobbyScreenState lobbyScreenStates[2];
};

struct GameState {
    int pressedStart;
    int threeDone;
    int twoDone;
    int oneDone;
    int goDone;
    int score;
    int score2;
    int goodVibes;
    int goodVibes2;

    int countdown;
    int flashTimer;
    int messageTimer;
    int messageTimer2;

    int earlyPoseA;
    int earlyPoseD;
    int latePoseA;
    int latePoseD;
    int nextPoseSendA;
    int nextPoseSendD;
    struct GameScreenState gameScreenStates[2];
};

struct ResultsState {
    int timer;
    char *winMessage;
    char *winMessageComplete;
    struct ResultsScreenState resultsScreenStates[2];
};

#endif