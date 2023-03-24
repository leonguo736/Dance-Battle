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
