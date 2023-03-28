#ifndef SONG_H
#define SONG_H

#include <stdio.h>
#include "pose.h"

#include "vgatools.h"

#define AUDIO_RATE 8000
#define NUM_SAMPLES 480000

struct Song {
    double samplesPerBeat;
    int sampleOffset;
    double * samplesL;
    double * samplesR;

    int numPoses;
    struct Pose * poses;
    struct ScreenPose * screenPoses;

    int sampleNo;
    int earlyPose;
    int latePose;
};

struct Song initSong(double spb, int so, double * sL, double * sR, int numPoses, struct Pose * poses);

#endif
