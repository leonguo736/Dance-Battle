#ifndef POSE_H
#define POSE_H

#include <stdio.h>
#include <stddef.h>

#define MAX_POSES 50

#define POSE_HOUR_LENGTH 30
#define POSE_MINUTE_LENGTH 50

struct Pose {
    double beat;
    double hourAngle;
    double minuteAngle;
};

#endif
