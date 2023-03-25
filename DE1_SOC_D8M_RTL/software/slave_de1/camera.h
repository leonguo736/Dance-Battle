#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

void writeThresholds(unsigned int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh); 

/*
 * deviceNumber:  0-9 are valid for deviceNumber
 */
void writeDeviceNumber(uint8_t deviceNumber); 

#define CAMERA_HW_MODULES 2
#define CAMERA_NUM_DETECTORS 2 // if CAMERA_NUM_DETECRORS > CAMERA_HW_MODULES, then the coords will be random
#define CAMERA_BUFFER_SIZE 3
#define CAMERA_DIMENSIONS 2

typedef struct CameraInterface {
    unsigned int buf[CAMERA_NUM_DETECTORS][CAMERA_BUFFER_SIZE][CAMERA_DIMENSIONS];
    unsigned int bufIndex; 
    uint8_t devId; 
} CameraInterface; 
CameraInterface* CameraInterface_new(uint8_t devId);
void CameraInterface_updateCoords(CameraInterface* cam); 
void CameraInterface_getMedian(CameraInterface* cam, unsigned int* median); 

#endif
