#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "camera.h"
#include "system.h"
#include "const.h"

volatile uint32_t *camera_base = (uint32_t*)COORDS_SLAVE_0_BASE;

void writeThresholds(unsigned int detectorNum, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh)
{
	*(camera_base + detectorNum) = (cbLow << 24) | (cbHigh << 16) | (crLow << 8) | crHigh;
}

void getCoordsFromHW(unsigned int detectorNum, unsigned int* x, unsigned int* y) {
    if (detectorNum < CAMERA_HW_MODULES) {
        uint32_t raw_coords = *(camera_base + detectorNum);
        uint16_t smallUpBigDown = (raw_coords >> 16) & 0xFFFF;
        uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
        if (smallLeftBigRight > 640 || smallUpBigDown > 480) {
            #ifdef DEBUG
            printf("Warning `getCoordsFromHW`: unrealistic coords on detector %i, raw: %lx, x: %u, y: %u\n", detectorNum, raw_coords, smallLeftBigRight, smallUpBigDown);
            #endif
        } else {            
            *x = smallLeftBigRight;
            *y = smallUpBigDown;
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
    cam->_bufIndex = 0;
    cam->_devId = devId;
    writeDeviceNumber(devId);
    return cam;
}

void CameraInterface_updateCoords(CameraInterface* cam) {
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        getCoordsFromHW(i, &cam->_buf[i][cam->_bufIndex][0], &cam->_buf[i][cam->_bufIndex][1]);
    }
    cam->_bufIndex = (cam->_bufIndex + 1) % CAMERA_BUFFER_SIZE;
}

void CameraInterface_updateMedian(CameraInterface* cam) {
    for (int i = 0; i < CAMERA_NUM_DETECTORS; i++) {
        unsigned int xValues[CAMERA_BUFFER_SIZE];
        unsigned int yValues[CAMERA_BUFFER_SIZE];
        for (int j = 0; j < CAMERA_BUFFER_SIZE; j++) {
            xValues[j] = cam->_buf[i][j][0];
            yValues[j] = cam->_buf[i][j][1];
        }
        unsigned int temp;
        for (int j = 0; j < CAMERA_BUFFER_SIZE; j++) {
            for (int k = j + 1; k < CAMERA_BUFFER_SIZE; k++) {
                if (xValues[j] > xValues[k]) {
                    temp = xValues[j];
                    xValues[j] = xValues[k];
                    xValues[k] = temp;
                }
                if (yValues[j] > yValues[k]) {
                    temp = yValues[j];
                    yValues[j] = yValues[k];
                    yValues[k] = temp;
                }
            }
        }
        if (CAMERA_BUFFER_SIZE % 2 == 0) {
            cam->median[i][0] = (xValues[CAMERA_BUFFER_SIZE/2] + xValues[CAMERA_BUFFER_SIZE/2 - 1]) / 2;
            cam->median[i][1] = (yValues[CAMERA_BUFFER_SIZE/2] + yValues[CAMERA_BUFFER_SIZE/2 - 1]) / 2;
        } else {
            cam->median[i][0] = xValues[CAMERA_BUFFER_SIZE/2];
            cam->median[i][1] = yValues[CAMERA_BUFFER_SIZE/2];
        }
    }
}

void CameraInterface_getJson(CameraInterface* ci, char* json_str, float beat) {
    char median_str[CAMERA_NUM_DETECTORS * CAMERA_DIMENSIONS * 10]; 
    char row_str[CAMERA_DIMENSIONS * 10];
    sprintf(median_str, "[[%d,%d]", ci->median[0][0], ci->median[0][1]);
    for (int i = 1; i < CAMERA_NUM_DETECTORS; i++) {
        sprintf(row_str, ",[%d,%d]", ci->median[i][0], ci->median[i][1]);
        strcat(median_str, row_str);
    }
    strcat(median_str, "]");
    sprintf(json_str, "{\"median\":%s,\"devId\":%d,\"beat\":%f},\"medianLen\":%d", median_str, ci->_devId, beat, CAMERA_NUM_DETECTORS);
}
