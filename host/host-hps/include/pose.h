#ifndef POSE_H
#define POSE_H

#include <stdio.h>
#include <stddef.h>

#define MAX_POSES 50

#define POSE_HOUR_LENGTH 25
#define POSE_MINUTE_LENGTH 40

struct Pose {
    double beat;
    double hourAngle;
    double minuteAngle;
};

#endif
