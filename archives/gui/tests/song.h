//#include <stdio.h>
//#include <stddef.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <pthread.h>
//#include <math.h>
//
//#define MAX_POSES 50
//#define MAX_SAMPLES 480000
//
//#define POSE_RADIUS 50
//#define POSE_HOUR_LENGTH 30
//#define POSE_MINUTE_LENGTH 50
//
//struct Pose {
//    double beat;
//    double hourAngle;
//    double minuteAngle;
//};
//
//struct ScreenPose {
//    int sample;
//    int x;
//    double hx;
//    double hy;
//    double mx;
//    double my;
//};
//
//struct Song {
//    double samplesPerBeat;
//    int sampleOffset;
//    double * samplesL;
//    double * samplesR;
//
//    int numPoses;
//    struct Pose * poses;
//    struct ScreenPose * screenPoses;
//
//    int sampleNo;
//    int earlyPose;
//    int latePose;
//};
//
//struct ScreenPose convertPose(struct Pose pose, double spb, int so) {
//    struct ScreenPose sp = { (int)(pose.beat * spb + so), -1, POSE_HOUR_LENGTH * cos(pose.hourAngle), POSE_HOUR_LENGTH * sin(pose.hourAngle), POSE_MINUTE_LENGTH * cos(pose.minuteAngle), POSE_MINUTE_LENGTH * sin(pose.minuteAngle) };
//
//    return sp;
//}
//
//struct Song initSong(double samplesPerBeat, int sampleOffset, double * samplesL, double * samplesR, int numPoses, struct Pose * poses) {
//
//    struct ScreenPose screenPoses[MAX_POSES];
//    for (int p = 0; p < numPoses; p++) {
//        screenPoses[p] = convertPose(poses[p], samplesPerBeat, sampleOffset);
//    }
//
//    struct Song song = { samplesPerBeat, sampleOffset, samplesL, samplesR, numPoses, poses, screenPoses, 0, -1, -1 };
//
//    return song;
//}
