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
#include "json.h"

#ifdef DEBUG
#include "cameraTest.h"
#include "jsonTest.h"
#endif

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

  // esp setup
  uart_init();
#ifdef DEBUG_ESP
  if (!esp_init(argc, argv))
  {
    printf("ESP Init failed\n");
    while (1)
      ;
  }
#endif

  // camera setup
  setupCameraThresholds();
  CameraInterface *cameraInterface = CameraInterface_new(6); // param is devId (DE1 slave number displayed on HEX)
  initCameraTimer(cameraInterface);

  // super loop
  while (1)
  {
    int uartReadLen = 10; // -1;
#ifndef ESP_DEBUG
    char *uartReadData = esp_read(&uartReadLen);
#else
    char *uartReadData = malloc(sizeof(*uartReadData) * uartReadLen);
#endif
    // while (1) {
    //   jsonTest();
    // }
    if (uartReadData != NULL)
    {
      free(uartReadData);
      
      char input[] = "echo,3,hello,world,!";
      int input_len = strlen(input);
      char **argv;
      char *cmd;
      int argc = arrayToArgs(input, input_len, &argv, &cmd);

      printf("command = %s\n", cmd);
      printf("argc = %d\n", argc);
      for (int i = 0; i < argc; i++)
      {
        printf("arg%d = %s\n", i, argv[i]);
      }

      // Free the dynamically allocated memory
      free(cmd);
      for (int i = 0; i < argc; i++)
      {
        free(argv[i]);
      }
      free(argv);

      int uartWriteLen = 500;
      char *uartWriteBuf = malloc(sizeof(*uartWriteBuf) * uartWriteLen);
      CameraInterface_updateMedian(cameraInterface);
      CameraInterface_getJson(cameraInterface, uartWriteBuf, 0.5);

#ifdef DEBUG
      printf("%s\n", uartWriteBuf);
#endif
      free(uartWriteBuf); // uartWrite(uartWriteBuf, uartWriteLen);
    }
  }
  free(cameraInterface);

#ifdef DEBUG
  printf("\n === Program end id: %i === \n", __programId__);
#endif

  return 0;
}
