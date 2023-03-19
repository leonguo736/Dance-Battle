#include "esp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hex.h"

struct Pose {
  double beat;
  double hourAngle;
  double minuteAngle;
};

void run(int argc, char** argv) {
  char recvBuffer[1024];
  char sendBuffer[1024];
  // char *sendPtr;
  // char *recvPtr;
  unsigned int count = 0;
  struct Pose pose;

  while (strcmp(recvBuffer, "connected") != 0) {
    uart_read_data(recvBuffer, 1024);
    //   recvPtr = recvBuffer;
    //   while (*recvPtr != '\n') {
    //     uart_read_data((unsigned int *)recvPtr);

    //     if (*recvPtr == '\n') {
    //       break;
    //     }
    //     recvPtr++;
    //     }
    //     *recvPtr = '\0';
    printf("%s\n", recvBuffer);
  }

  uart_write_data("start\n");
  // uart_write_data('s');
  // uart_write_data('t');
  // uart_write_data('a');
  // uart_write_data('r');
  // uart_write_data('t');
  // uart_write_data('\r');

  while (count < 10) {
    display(count);

    pose.beat = (rand() % 1000) / (rand() % 100 + 1.0);
    pose.hourAngle = (rand() % 1000) / (rand() % 100 + 1.0);
    pose.minuteAngle = (rand() % 1000) / (rand() % 100 + 1.0);

    sprintf(sendBuffer, "%f,%f,%f", pose.beat, pose.hourAngle,
            pose.minuteAngle);

    uart_write_data(sendBuffer);
    // sendPtr = sendBuffer;
    // while (*sendPtr != '\0') {
    //     uart_write_data((unsigned int) *sendPtr);
    //     sendPtr++;
    // }

    // uart_write_data('\r');

    unsigned int len = uart_read_data(recvBuffer, 1024);

    // recvPtr = recvBuffer;
    // while (*recvPtr != '\n') {
    //     uart_read_data((unsigned int *)recvPtr);

    //     if (*recvPtr == '\n') {
    //         break;
    //     }
    //     recvPtr++;
    // }
    // *recvPtr = '\0';
    printf("[%d] %s\n", len, recvBuffer);
    count++;
  }

  while (1) {
    unsigned int len = uart_read_data(recvBuffer, 1024);
    // recvPtr = recvBuffer;
    // while (*recvPtr != '\n') {
    //     uart_read_data((unsigned int *)recvPtr);

    //     if (*recvPtr == '\n') {
    //         break;
    //     }
    //     recvPtr++;
    // }
    // *recvPtr = '\0';
    printf("[%d] %s\n", len, recvBuffer);
  }
}
