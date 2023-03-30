#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "system.h"
#include "esp.h"
#include "uart.h"
#include "const.h"
#include "timer.h"
#include "camera.h"

#ifdef DEBUG
#include "cameraTest.h"
#endif

void startUart(int argc, char **argv)
{
  uart_init();
  uart_output();

  if (!esp_init(argc, argv))
  {
    while (1)
      ;
    return;
  };

  esp_run();
  // pid_t pid = fork();

  // if (pid == -1) {
  //   printf("Fork failed\n");
  //   while(1)
  //     ;
  //   return 1;
  // } else if (pid == 0) {
  //   esp_run();
  // } else {
  //   printf("Parent Process\n");
  //   while (1) {
  //     updateCoords();
  //     for (int i = 0; i < NUM_POINT_FINDERS; i++) {
  //       uint32_t raw_coords = coords[i];
  //       uint16_t smallUpBigDown = raw_coords >> 16;
  //       uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
  //       printf("%i: (%i, %i), ", i, smallLeftBigRight, smallUpBigDown);
  //     }
  //     printf("\n");
  //   }
  // }
}

void setupCameraThresholds()
{
  writeThresholds(0, 146, 160, 61, 85); // Leon's Blue
  writeThresholds(1, 64, 93, 156, 175); // Kerry's Dark Red
  for (int i = 1; i < CAMERA_NUM_DETECTORS; i++)
  {
    writeThresholds(i, 64, 93, 156, 175); // Kerry's Dark Red
  }
}

int main(int argc, char **argv)
{
#ifdef DEBUG
  srand(time(NULL));
  const unsigned int __programId__ = rand() % 100;
  printf("\n === Program start id: %i === \n", __programId__);
#endif

  // uart setup
  //  startUart(argc, argv);

  // camera setup
  setupCameraThresholds();
  CameraInterface *cameraInterface = CameraInterface_new(6); // param is devId (DE1 slave number displayed on HEX)
  initCameraTimer(cameraInterface);

  // super loop
  while (1)
  {
    int uartReadLen = 420; // -1;
    char *uartReadData = NULL; // uartRead(&uartReadLen);
    if (uartReadLen > 0) {
      (void)uartReadData; // TODO: parse uartReadData
      int uartWriteLen = 500; 
      char *uartWriteBuf = malloc(sizeof(*uartWriteBuf) * uartWriteLen); 
      CameraInterface_updateMedian(cameraInterface);
      CameraInterface_getJson(cameraInterface, uartWriteBuf, 0.5); 
      printf("%s\n", uartWriteBuf);
      free(uartWriteBuf); // uartWrite(uartWriteBuf, uartWriteLen);
    }
  }
  free(cameraInterface);

#ifdef DEBUG
  printf("\n === Program end id: %i === \n", __programId__);
#endif

  return 0;
}
