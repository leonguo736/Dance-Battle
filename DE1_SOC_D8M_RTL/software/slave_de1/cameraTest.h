#ifndef CAMERATEST_H
#define CAMERATEST_H

#include <stdio.h>
#include "camera.h"

int cameraTest() {
    srand(time(NULL));
    struct CameraInterface *camera_interface = CameraInterface_new();
    for (int i = 0; i < BUFFER_SIZE; i++) {
        update(camera_interface);
    }
    printf("Buffer:\n");
    for (int i = 0; i < NUM_DETECTORS; i++) {
        for (int j = 0; j < BUFFER_SIZE; j++) {
            printf("%f %f\n", camera_interface->buf[i][j][0], camera_interface->buf[i][j][1]);
        }
    }
    float median[NUM_DETECTORS * DIMENSIONS];
    getMedian(camera_interface, median);
    printf("Median:\n");
    for (int i = 0; i < NUM_DETECTORS; i++) {
        for (int j = 0; j < DIMENSIONS; j++) {
            printf("%f ", median[i * DIMENSIONS + j]);
        }
        printf("\n");
    }
    return 0;
} 

#endif