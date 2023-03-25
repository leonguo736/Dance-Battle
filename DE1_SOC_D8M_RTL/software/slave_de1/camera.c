#include <stdlib.h>
#include <stdio.h>
#include "camera.h"
#include "system.h"

volatile uint32_t *camera_base = (uint32_t*)COORDS_SLAVE_0_BASE;

void writeThresholds(unsigned int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh)
{
	*(camera_base + i) = (cbLow << 24) | (cbHigh << 16) | (crLow << 8) | crHigh;
}

void getCoordsFromHW(unsigned int detectorNum, unsigned int* x, unsigned int* y) {
    if (detectorNum < CAMERA_HW_MODULES) {
        uint32_t raw_coords = *(camera_base + detectorNum);
        uint16_t smallUpBigDown = (raw_coords >> 16) & 0xFFFF;
        uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
        *x = smallLeftBigRight;
        *y = smallUpBigDown;
        if (smallLeftBigRight > 800) {
            printf("Warning: x coord is too large, raw_coords: %x, x: %i, y: %i\n", raw_coords, *x, *y);
        }
    } else {
        *x = rand() % 100; 
        *y = rand() % 100;
    }
}

void writeDeviceNumber(uint8_t devId)
{
  if (devId > 9) {
    printf("Error `writeDeviceNumber`: devId must be between 0 and 9 inclusive\n");
    return;
  }
  *(camera_base + 31) = (uint32_t) devId;
}

CameraInterface* CameraInterface_new(uint8_t devId) {
    CameraInterface* cam = (CameraInterface*) malloc(sizeof(CameraInterface));
    cam->bufIndex = 0;
    cam->devId = devId;
    writeDeviceNumber(devId);
    return cam;
}

void CameraInterface_updateCoords(CameraInterface* cam) {
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        getCoordsFromHW(i, &cam->buf[i][cam->bufIndex][0], &cam->buf[i][cam->bufIndex][1]);
    }
    cam->bufIndex = (cam->bufIndex + 1) % CAMERA_BUFFER_SIZE;
}

void CameraInterface_getMedian(CameraInterface* cam, unsigned int* median) {
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        for (int j = 0; j < CAMERA_DIMENSIONS; j++) {
            unsigned int values[CAMERA_BUFFER_SIZE];
            for (int k = 0; k < CAMERA_BUFFER_SIZE; k++) {
                values[k] = cam->buf[i][k][j];
            }
            unsigned int temp;
            for (int k = 0; k < CAMERA_BUFFER_SIZE; k++) {
                for (int l = k + 1; l < CAMERA_BUFFER_SIZE; l++) {
                    if (values[k] > values[l]) {
                        temp = values[k];
                        values[k] = values[l];
                        values[l] = temp;
                    }
                }
            }
            if (CAMERA_BUFFER_SIZE % 2 == 0) {
                median[i * CAMERA_DIMENSIONS + j] = (values[CAMERA_BUFFER_SIZE/2] + values[CAMERA_BUFFER_SIZE/2 - 1]) / 2;
            } else {
                median[i * CAMERA_DIMENSIONS + j] = values[CAMERA_BUFFER_SIZE/2];
            }
        }
    }
}
