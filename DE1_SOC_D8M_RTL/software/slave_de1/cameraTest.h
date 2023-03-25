#ifndef CAMERATEST_H
#define CAMERATEST_H

#include <stdio.h>
#include <stdint.h>
#include "camera.h"

int cameraTest() {
    srand(time(NULL));
    CameraInterface *camera_interface = CameraInterface_new(6);
    for (int i = 0; i < CAMERA_BUFFER_SIZE; i++) {
        CameraInterface_updateCoords(camera_interface);
    }
    printf("Buffer:\n");
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        for (int j = 0; j < CAMERA_BUFFER_SIZE; j++) {
            printf("%i %i\n", camera_interface->buf[i][j][0], camera_interface->buf[i][j][1]);
        }
    }
    unsigned int median[CAMERA_NUM_DETECTORS * CAMERA_DIMENSIONS];
    CameraInterface_getMedian(camera_interface, median);
    printf("Median:\n");
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        for (int j = 0; j < CAMERA_DIMENSIONS; j++) {
            printf("%i ", median[i * CAMERA_DIMENSIONS + j]);
        }
        printf("\n");
    }
    return 0;
} 

#endif