#ifndef SONG_H
#define SONG_H

#include <stdio.h>
#include "pose.h"

#include "vgatools.h"

#define AUDIO_RATE 48000 // If you change this, recalculate SAMPLES_PER_FRAME
#define NUM_SAMPLES 1000000
#define VOLUME 0x7FFFFFF

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
