#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "esp.h"
#include "uart.h"
#include "system.h"

#define NUM_POINT_FINDERS 2
extern uint32_t coords[NUM_POINT_FINDERS]; 

void writeThresholds(int i, uint8_t cbLow, uint8_t cbHigh, uint8_t crLow, uint8_t crHigh); 

void updateCoords(); 

/*
 * deviceNumber:  0-9 are valid for deviceNumber
 */
void writeDeviceNumber(uint8_t deviceNumber); 

#endif