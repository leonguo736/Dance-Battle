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
  int reduceRange = 4;
  //  writeThresholds(0, 84+reduceRange, 104-reduceRange, 137+reduceRange, 157-reduceRange); // Yellow Foam
  //  writeThresholds(1, 110+reduceRange, 130-reduceRange, 143+reduceRange, 163-reduceRange); // Red Foam
  //  writeThresholds(2, 109+reduceRange, 129-reduceRange, 127+reduceRange, 147-reduceRange); // Brown Foam
  //  writeThresholds(3, 84+reduceRange, 104-reduceRange, 137+reduceRange, 157-reduceRange); // Yellow Foam
  //  writeThresholds(4, 110+reduceRange, 130-reduceRange, 143+reduceRange, 163-reduceRange); // Red Foam
  //  writeThresholds(5, 109+reduceRange, 129-reduceRange, 127+reduceRange, 147-reduceRange); // Brown Foam
}

int main(int argc, char **argv)
{
  DEBUGPRINT("Running Main\n");

  // camera setup
  setupCameraThresholds();
  CameraInterface *cameraInterface = CameraInterface_new(9); // param is devId (DE1 slave number displayed on HEX)
  DEBUGPRINT("INFO: Initializing timer (quartus reprogram if not done) ... ");
  initCameraTimer(cameraInterface);
  DEBUGPRINT("done\n");

#ifndef ESP_DEBUG
  uart_init();
  DEBUGPRINT("INFO: UART initialized\n");
  if (!esp_init(argc, argv))
  {
    DEBUGPRINT("ERROR: ESP Init failed\n");
    while (1)
      ;
  }
#else
  DEBUGPRINT("WARNING: ESP_DEBUG enabled\n");
#endif

#ifndef ESP_DEBUG
  esp_write("{\"command\":\"setType\",\"identifier\":\"camera\"}");
#endif

  // super loop
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
      char text[] = "cap,420";
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
    // DEBUGPRINT("INFO: while\n");
    if (uartReadData != NULL)
    {
      // Parse data from backend
      char *cmd = strtok(uartReadData, ",");
      if (cmd == NULL)
      {
        DEBUGPRINT("ERROR: cmd is NULL\n");
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
        DEBUGPRINT("INFO: cmd id %i\n", atoi(data));
        writeDeviceNumber(atoi(data));
      }
      else if (strcmp(cmd, "cap") == 0)
      {
        int cameraWriteLen = 500;
        char *cameraWriteBuf = malloc(sizeof(*cameraWriteBuf) * cameraWriteLen);
        CameraInterface_updateMedian(cameraInterface);
        CameraInterface_getMedianStr(cameraInterface, cameraWriteBuf);

        char *uartWriteBuf = malloc(sizeof(*uartWriteBuf) * cameraWriteLen);
        sprintf(uartWriteBuf, "{%s,\"poseId\":%s,\"command\":\"poseData\"}", cameraWriteBuf, data);

        DEBUGPRINT("INFO: cmd cap, sent %s\n", uartWriteBuf);

        esp_write(uartWriteBuf);
        free(cameraWriteBuf);
        free(uartWriteBuf);
      }
      else if (strcmp(cmd, "c") == 0)
      {
        DEBUGPRINT("WARNING: esp disconnected, reconnecting ... \n");
        writeDeviceNumber(9);
        if (!esp_init(argc, argv))
        {
          DEBUGPRINT("failed\n");
          while (1)
            ;
        }
        esp_write("{\"command\":\"setType\",\"identifier\":\"camera\"}");
      }
      else
      {
        DEBUGPRINT("WARNING: unknown cmd %s, data %s\n", cmd, data);
      }
      free(data);
      free(uartReadData);
    }
  }
  free(cameraInterface);

  return 0;
}
