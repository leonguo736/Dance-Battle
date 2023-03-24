#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"
#include "esp.h"
#include "uart.h"
#include "camera.h"
#include "const.h"

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

void startCamera()
{
//  writeThresholds(0, 0, 255, 0, 255);
  writeThresholds(0, 146, 160, 61, 85); // Leon's Blue
  writeThresholds(1, 64, 93, 156, 175); // Kerry's Dark Red
  writeDeviceNumber(8);
#ifdef DEBUG
  while (1)
  {
    updateCoords();
    for (int i = 0; i < NUM_POINT_FINDERS; i++)
    {
      uint32_t raw_coords = coords[i];
      uint16_t smallUpBigDown = raw_coords >> 16;
      uint16_t smallLeftBigRight = raw_coords & 0xFFFF;
      printf("%i: (%i, %i), ", i, smallLeftBigRight, smallUpBigDown);
    }
    printf("\n");
  }
#endif
}

int main(int argc, char **argv)
{
#ifdef DEBUG
  const int __programNumber__ = 420;
  printf("\n === Program start number: %i === \n", __programNumber__);
#endif

//  startUart(argc, argv);
  startCamera();

#ifdef DEBUG
  printf("\n === Program end number: %i === \n", __programNumber__);
#endif

  return 0;
}
