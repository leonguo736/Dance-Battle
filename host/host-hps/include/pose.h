#ifndef POSE_H
#define POSE_H

#include <stdio.h>
#include <stddef.h>

#define MAX_POSES 100

#define POSE_HOUR_LENGTH 25
#define POSE_MINUTE_LENGTH 40

#define POSE_LINE_ATTACK_Y1 -30
#define POSE_LINE_ATTACK_Y2 20
#define POSE_LINE_DEFEND_Y1 -10
#define POSE_LINE_DEFEND_Y2 10

#define POSE_HEAD_Y -20
#define POSE_CHEST_Y -10
#define POSE_PELVIS_Y 10
#define POSE_ARM_LENGTH 20
#define POSE_LEG_LENGTH 25
#define POSE_SHOULDER_LENGTH 3

#define COLOR_POSE_BODY 0xffff
#define COLOR_POSE_LARM 0xf986
#define COLOR_POSE_RARM 0xffe6
#define COLOR_POSE_LLEG 0x319f
#define COLOR_POSE_RLEG 0x37ff
#define COLOR_ATTACK_ICON 0xf98c
#define COLOR_DEFEND_ICON 0x34bf

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
