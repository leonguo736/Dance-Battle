#ifndef CAMERATEST_H
#define CAMERATEST_H

#include <stdio.h>
#include <stdint.h>
#include "camera.h"

int cameraTest() {
    // uint1
    // printf("getCoordsFromHW(0), ")
    srand(time(NULL));
    struct CameraInterface *camera_interface = CameraInterface_new();
    for (int i = 0; i < BUFFER_SIZE; i++) {
        update(camera_interface);
    }
    printf("Buffer:\n");
    for (int i = 0; i < NUM_DETECTORS; i++) {
        for (int j = 0; j < BUFFER_SIZE; j++) {
            printf("%i %i\n", camera_interface->buf[i][j][0], camera_interface->buf[i][j][1]);
        }
    }
    int median[NUM_DETECTORS * DIMENSIONS];
    getMedian(camera_interface, median);
    printf("Median:\n");
    for (int i = 0; i < NUM_DETECTORS; i++) {
        for (int j = 0; j < DIMENSIONS; j++) {
            printf("%i ", median[i * DIMENSIONS + j]);
        }
        printf("\n");
    }
    return 0;
} 

#endif