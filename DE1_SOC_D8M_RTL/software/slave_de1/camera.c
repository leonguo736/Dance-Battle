#include "camera.h"
#include "system.h"

volatile uint32_t *camera_base = (uint32_t*)COORDS_SLAVE_0_BASE;

void writeThresholds(int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh)
{
	*(camera_base + i) = (cbLow << 24) | (cbHigh << 16) | (crLow << 8) | crHigh;
}

void getCoordsFromHW(int detectorNum, int* x, int* y) {
    if (detectorNum < CAMERA_HW_MODULES) {
        uint32_t raw_coords = *(camera_base + detectorNum);
        uint16_t smallUpBigDown = (raw_coords >> 16) & 0xFFFF;
        uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
        *x = smallLeftBigRight;
        *y = smallUpBigDown;
    } else {
        *x = rand() % 100; 
        *y = rand() % 100;
    }
}

// Only 0-9 are valid for devId
void writeDeviceNumber(uint8_t devId)
{
  if (devId > 9) {
    printf("Device id must be between 0 and 9 inclusive\n");
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

void CameraInterface_update(CameraInterface* cam) {
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        getCoordsFromHW(i, &cam->buf[i][cam->bufIndex][0], &cam->buf[i][cam->bufIndex][1]);
    }
    cam->bufIndex = (cam->bufIndex + 1) % CAMERA_BUFFER_SIZE;
}

void CameraInterface_getMedian(CameraInterface* cam, int* median) {
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        for (int j = 0; j < CAMERA_DIMENSIONS; j++) {
            int values[CAMERA_BUFFER_SIZE];
            for (int k = 0; k < CAMERA_BUFFER_SIZE; k++) {
                values[k] = cam->buf[i][k][j];
            }
            int temp;
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
