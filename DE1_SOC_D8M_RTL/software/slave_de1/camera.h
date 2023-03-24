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

#define NUM_POINT_FINDERS 6
extern uint32_t coords[NUM_POINT_FINDERS]; 

void writeThresholds(int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh); 

void updateCoords(); 

/*
 * deviceNumber:  0-9 are valid for deviceNumber
 */
void writeDeviceNumber(uint8_t deviceNumber); 

#define NUM_DETECTORS 2
#define BUFFER_SIZE 3
#define DIMENSIONS 2
struct CameraInterface {
    float buf[NUM_DETECTORS][BUFFER_SIZE][DIMENSIONS];
    int buf_idx; 
}; 
struct CameraInterface* CameraInterface_new(); 
void update(struct CameraInterface* cam); 
void getMedian(struct CameraInterface* cam, float* median); 

#endif
