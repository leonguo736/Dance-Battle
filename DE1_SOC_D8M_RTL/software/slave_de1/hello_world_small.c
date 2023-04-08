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
  int reduceRange = 6;
  writeThresholds(0, 84+reduceRange, 104-reduceRange, 137+reduceRange, 157-reduceRange); // Yellow Foam
  writeThresholds(1, 110+reduceRange, 130-reduceRange, 143+reduceRange, 163-reduceRange); // Red Foam
  writeThresholds(2, 109+reduceRange, 129-reduceRange, 127+reduceRange, 147-reduceRange); // Brown Foam

  // for (int i = 1; i < CAMERA_NUM_DETECTORS; i++)
  // {
  //   writeThresholds(i, 64, 93, 156, 175); // Kerry's Dark Red
  // }
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

  printf("INFO: ESP initialized\n");
  // camera setup
  setupCameraThresholds();
  CameraInterface *cameraInterface = CameraInterface_new(6); // param is devId (DE1 slave number displayed on HEX)
  initCameraTimer(cameraInterface);

  // super loop
#ifndef ESP_DEBUG
  esp_write("{\"command\":\"setType\",\"identifier\":\"camera\"}");
#endif
  while (1)
  {
    unsigned int uartReadLen = 10; // -1;
#ifndef ESP_DEBUG
    char *uartReadData = esp_read(&uartReadLen);
#else
    static unsigned int randomCounter = 0;
    char *uartReadData;
    if (++randomCounter % 2 == 0)
    {
      char text[] = "cap";
      uartReadLen = strlen(text);
      uartReadData = malloc(sizeof(*uartReadData) * uartReadLen);
      memcpy(uartReadData, text, uartReadLen);
    }
    else
    {
      char text[] = "id,8";
      uartReadLen = strlen(text);
      uartReadData = malloc(sizeof(*uartReadData) * uartReadLen);
      memcpy(uartReadData, text, uartReadLen);
    }
#endif
    if (uartReadData != NULL)
    {
      printf("INFO `main`: uartReadData %s, uartReadLen %i\n", uartReadData, uartReadLen);

      // Parse data from backend
      char *cmd = strtok(uartReadData, ",");
      if (cmd == NULL)
      {
        printf("ERROR `main`: cmd is NULL\n");
        free(uartReadData);
        continue;
      }

      int data_length = uartReadLen - strlen(cmd) - 1;
      char *data = NULL;
      if (data_length > 0)
      {
        data = malloc((data_length + 2) * sizeof(*data));
        memcpy(data, uartReadData + strlen(cmd) + 1, data_length);
        data[data_length] = '\0';
      }
      if (strcmp(cmd, "id") == 0)
      {
        printf("INFO `main`: cmd %s, data %s\n", cmd, data);
        writeDeviceNumber(atoi(data));
      }
      else if (strcmp(cmd, "cap") == 0)
      {
        int uartWriteLen = 500;
        char *cameraWriteBuf = malloc(sizeof(*cameraWriteBuf) * uartWriteLen);
        CameraInterface_updateMedian(cameraInterface);
        CameraInterface_getMedianStr(cameraInterface, cameraWriteBuf);        
        
        char *uartWriteBuf = malloc(sizeof(*uartWriteBuf) * uartWriteLen);
        sprintf(uartWriteBuf, "{%s,\"poseId\":%s,\"command\":\"poseData\"}", cameraWriteBuf, data);

        printf("INFO `main`: cmd %s, uartWriteBuf %s\n", cmd, uartWriteBuf);

        esp_write(uartWriteBuf);
        free(cameraWriteBuf);
        free(uartWriteBuf);
      } else if (strcmp(cmd, "c") == 0) {
        printf("WARNING `main` esp disconnected"); 
        esp_init(argc, argv);
      }
      else {
        printf("WARNING `main` unknown cmd: cmd %s, data %s\n", cmd, data);
      }
      free(data);
      free(uartReadData);
    }
  }
  free(cameraInterface);

#ifdef DEBUG
  printf("\n === Program end id: %i === \n", __programId__);
#endif

  return 0;
}
