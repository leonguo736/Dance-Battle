#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "esp.h"
#include "uart.h"
#include "system.h"

void writeThresholds(int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh); 

/*
 * deviceNumber:  0-9 are valid for deviceNumber
 */
void writeDeviceNumber(uint8_t deviceNumber); 

#define CAMERA_HW_MODULES 2
#define CAMERA_NUM_DETECTORS 8 // if CAMERA_NUM_DETECRORS > CAMERA_HW_MODULES, then the coords will be random
#define CAMERA_BUFFER_SIZE 3
#define CAMERA_DIMENSIONS 2

typedef struct CameraInterface {
    int buf[CAMERA_NUM_DETECTORS][CAMERA_BUFFER_SIZE][CAMERA_DIMENSIONS];
    int bufIndex; 
    uint8_t devId; 
} CameraInterface; 
CameraInterface* CameraInterface_new(uint8_t devId);
void CameraInterface_update(CameraInterface* cam); 
void CameraInterface_getMedian(CameraInterface* cam, int* median); 

#endif
