#include "camera.h"
#include "system.h"

volatile uint32_t *camera_base = (uint32_t*)COORDS_SLAVE_0_BASE;
uint32_t coords[NUM_POINT_FINDERS]; 

void writeThresholds(int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh)
{
	*(camera_base + i) = (cbLow << 24) | (cbHigh << 16) | (crLow << 8) | crHigh;
}

void updateCoords() {	
	for (int i = 0; i < NUM_POINT_FINDERS; i++) {
		coords[i] = *(camera_base + i);
	}
}

// Only 0-9 are valid for deviceNumber
void writeDeviceNumber(uint8_t deviceNumber)
{
  if (deviceNumber > 9) {
    printf("Device number must be between 0 and 9 inclusive\n");
    return;
  }
  *(camera_base + 31) = (uint32_t) deviceNumber;
}

float fetchCameraData(int i) {
    return (float)(rand() % 1000);
}

struct CameraInterface* CameraInterface_new() {
    struct CameraInterface* cam = (struct CameraInterface*) malloc(sizeof(struct CameraInterface));
    cam->buf_idx = 0;
    return cam;
}

void update(struct CameraInterface* cam) {
    for (int i = 0; i < NUM_DETECTORS; i++) {
        cam->buf[i][cam->buf_idx][0] = fetchCameraData(i);
        cam->buf[i][cam->buf_idx][1] = fetchCameraData(i);
    }
    cam->buf_idx = (cam->buf_idx + 1) % BUFFER_SIZE;
}

void getMedian(struct CameraInterface* cam, float* median) {
    for (int i = 0; i < NUM_DETECTORS; i++) {
        for (int j = 0; j < DIMENSIONS; j++) {
            float values[BUFFER_SIZE];
            for (int k = 0; k < BUFFER_SIZE; k++) {
                values[k] = cam->buf[i][k][j];
            }
            float temp;
            for (int k = 0; k < BUFFER_SIZE; k++) {
                for (int l = k + 1; l < BUFFER_SIZE; l++) {
                    if (values[k] > values[l]) {
                        temp = values[k];
                        values[k] = values[l];
                        values[l] = temp;
                    }
                }
            }
            if (BUFFER_SIZE % 2 == 0) {
                median[i * DIMENSIONS + j] = (values[BUFFER_SIZE/2] + values[BUFFER_SIZE/2 - 1]) / 2;
            } else {
                median[i * DIMENSIONS + j] = values[BUFFER_SIZE/2];
            }
        }
    }
}
