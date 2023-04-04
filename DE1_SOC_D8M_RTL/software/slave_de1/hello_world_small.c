#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
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

#ifndef ESP_DEBUG
  uart_init();
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
    unsigned int uartReadLen = 10; // -1;
#ifndef ESP_DEBUG
    char *uartReadData = esp_read(&uartReadLen);
#else
    static unsigned int randomCounter = 0; 
    char *uartReadData; 
    if (++randomCounter % 2 == 0) { 
      char text[] = "cap"; 
      uartReadLen = strlen(text);
      uartReadData = malloc(sizeof(*uartReadData) * uartReadLen);
      memcpy(uartReadData, text, uartReadLen);
    } else { 
      char text[] = "id,2";
      uartReadLen = strlen(text);
      uartReadData = malloc(sizeof(*uartReadData) * uartReadLen);
      memcpy(uartReadData, text, uartReadLen);
    }
#endif
    if (uartReadData != NULL)
    {
      uartReadData[uartReadLen] = '\0';
      char *cmd, *data;
      cmd = strtok(uartReadData, ",");
      int data_length = uartReadLen - strlen(cmd) - 1;
      printf("INFO `main`: command %s\n", cmd);
      data = NULL;
      if (data_length > 0)
      {
        data = malloc((data_length + 2) * sizeof(*data));
        memcpy(data, uartReadData + strlen(cmd) + 1, data_length);
        data[data_length] = '\0';
        printf("INFO `main`: data %s\n", data);
      }
      if (strcmp(cmd, "id") == 0)
      {
        writeDeviceNumber(atoi(data));
      } else if (strcmp(cmd, "cap") == 0)
      {
        int uartWriteLen = 500;
        char *uartWriteBuf = malloc(sizeof(*uartWriteBuf) * uartWriteLen);
        CameraInterface_updateMedian(cameraInterface);
        CameraInterface_getJson(cameraInterface, uartWriteBuf, 0.5);

        esp_write(uartWriteBuf);
        free(uartWriteBuf); // uartWrite(uartWriteBuf, uartWriteLen);
      }

#ifdef DEBUG
      // printf("%s\n", uartWriteBuf);
#endif
    }
  }
  free(cameraInterface);

#ifdef DEBUG
  printf("\n === Program end id: %i === \n", __programId__);
#endif

  return 0;
}
