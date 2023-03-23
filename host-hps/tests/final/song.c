#include <stdio.h>

#include "song.h"

struct Song initSong(double spb, int so, double * sL, double * sR, int numPoses, struct Pose * poses) {

    struct ScreenPose screenPoses[numPoses];
    for (int p = 0; p < numPoses; p++) {
        screenPoses[p] = convertPose(poses[p], spb, so);
    }

    struct Song song = { spb, so, sL, sR, numPoses, poses, screenPoses, 0, -1, -1 };
    return song;
}