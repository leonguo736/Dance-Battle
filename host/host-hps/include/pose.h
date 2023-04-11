#ifndef POSE_H
#define POSE_H

#include <stdio.h>
#include <stddef.h>

#define MAX_POSES 50

#define POSE_HOUR_LENGTH 25
#define POSE_MINUTE_LENGTH 40

#define POSE_CHEST_Y 10
#define POSE_PELVIS_Y -10
#define POSE_ARM_LENGTH 25
#define POSE_LEG_LENGTH 30

struct Pose {
    int isDefender;
    int sample;

    double larmx;
    double larmy;
    double rarmx;
    double rarmy;
    double llegx;
    double llegy;
    double rlegx;
    double rlegy;
};

#endif
