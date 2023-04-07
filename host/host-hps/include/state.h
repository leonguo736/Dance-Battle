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
    int pBarWidth;
    int earlyPose;
    int latePose;
    int poseXs[MAX_POSES];
};

struct ResultsScreenState {

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
    struct ScreenPose screenPoses[MAX_POSES];
    int sampleNo;
    int earlyPose;
    int latePose;
    int gameStarted;
    struct GameScreenState gameScreenStates[2];
};

struct ResultsState {
    struct ResultsScreenState resultsScreenStates[2];
};

#endif