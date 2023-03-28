#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "camera.h"
#include "const.h"
#include "esp.h"
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"
#include "system.h"
#include "timer.h"
#include "uart.h"

#ifdef DEBUG
#include "cameraTest.h"
#endif

static void init_sysclk() { 
  alt_sysclk_init(SYS_CLK_TICKS_PER_SEC); 
}

void startUart(int argc, char **argv) {
  uart_init();
  uart_output();
  if (alt_timestamp_start() < 0) {
    printf("No timestamp device\n");
  }
  struct timeval start;
  struct timeval end;

  alt_timestamp_type timestamp_start;
  alt_timestamp_type timestamp_end;

  int ticks_start;
  int ticks_end;

  // struct timespec start1;
  // struct timespec end1;
  int setup = gettimeofday(&start, NULL);
  timestamp_start = alt_timestamp();
  ticks_start = alt_nticks();
  printf("Setup: %d\n", setup);
  // clock_gettime(CLOCK_REALTIME, &start1);

  printf("TimeOfDay Start: %f ms\n",
         (double)(start.tv_sec * SECONDS_TO_MILLISECONDS +
                  start.tv_usec * MICROSECONDS_TO_MILLISECONDS));
  printf("Timestamp_Start: %d\n", timestamp_start);
  printf("Ticks_Start: %d\n", ticks_start);
  // printf("Timespec Start: %f ms\n",
  //        (double)(start1.tv_sec * SECONDS_TO_MILLISECONDS +
  //                 start1.tv_nsec * NANOSECONDS_TO_MILLISECONDS));

  if (!esp_init(argc, argv)) {
    // while (1)
    //   ;
    // return;
  };

  // Calc diff
  time_t start_clock = clock();
  gettimeofday(&end, NULL);
  timestamp_end = alt_timestamp();
  ticks_end = alt_nticks();
  // clock_gettime(CLOCK_REALTIME, &end1);
  printf("TimeOfDay End: %f ms\n",
         (double)(end.tv_sec * SECONDS_TO_MILLISECONDS +
                  end.tv_usec * MICROSECONDS_TO_MILLISECONDS));
  printf("Timestamp_End: %d\n", timestamp_end);
  printf("Ticks_End: %d\n", ticks_end);
  // printf("Timespec End: %f ms\n",
  //        (double)(end1.tv_sec * SECONDS_TO_MILLISECONDS +
  //                 end1.tv_nsec * NANOSECONDS_TO_MILLISECONDS));
  printf(
      "TimeOfDay Time taken: %f ms\n",
      (double)((end.tv_sec - start.tv_sec) * SECONDS_TO_MILLISECONDS +
               (end.tv_usec - start.tv_usec) * MICROSECONDS_TO_MILLISECONDS));
  printf("Timestamp Time taken: %d | %f ms\n", timestamp_end - timestamp_start,
         (double)(timestamp_end - timestamp_start) /
             (alt_timestamp_freq() * MICROSECONDS_TO_MILLISECONDS));
  printf("Ticks Time taken: %d | %f s | %f ms\n", ticks_end - ticks_start,
         (double)(ticks_end - ticks_start) / (alt_ticks_per_second()),
         ((double)(ticks_end - ticks_start) / (alt_ticks_per_second())) * SECONDS_TO_MILLISECONDS);
  // printf(
  //     "Timespec Time taken: %f ms\n",
  //     (double)((end1.tv_sec - start1.tv_sec) * SECONDS_TO_MILLISECONDS +
  //              (end1.tv_nsec - start1.tv_nsec) *
  //              NANOSECONDS_TO_MILLISECONDS));

  esp_run();
  while (1)
    ;
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

void startCamera() {
  writeThresholds(0, 146, 160, 61, 85);  // Leon's Blue
  writeThresholds(1, 64, 93, 156, 175);  // Kerry's Dark Red
  for (int i = 1; i < CAMERA_NUM_DETECTORS; i++) {
    writeThresholds(i, 64, 93, 156, 175);  // Kerry's Dark Red
  }

  CameraInterface *cameraInterface = CameraInterface_new(
      6);  // param is devId (DE1 slave number displayed on HEX)
  initCameraTimer(cameraInterface);
  // initCameraTimer(6); // param is devId (DE1 slave number displayed on HEX)
#ifdef DEBUG
  while (1) {
    // CameraInterface* cameraInterface = getTimerCameraInterface();
    CameraInterface_updateMedian(cameraInterface);
    char json_str[500];
    CameraInterface_getJson(cameraInterface, json_str);
    printf("%s\n", json_str);
    usleep(100000);
  }
  free(cameraInterface);
#endif
}

int main(int argc, char **argv) {
  #ifdef DEBUG
  srand(time(NULL));
  const unsigned int __programId__ = rand() % 100;
  printf("\n === Program start id: %i === \n", __programId__);
  #endif

  init_sysclk();
  startUart(argc, argv);
  // startCamera();
  //  cameraTest();
  //  testTimer();

#ifdef DEBUG
  printf("\n === Program end id: %i === \n", __programId__);
#endif

  return 0;
}
