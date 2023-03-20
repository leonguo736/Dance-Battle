#include "esp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "hex.h"

#define SERVER_IP "192.168.0.198:8080"

struct Pose {
  double beat;
  double hourAngle;
  double minuteAngle;
};

bool init_connection(void);

// Function Definitions
bool init_connection(void) {
  // Reset the ESP
  uart_send_command(ESP_RESET_COMMAND, NULL, 0);
  uart_wait_for_messages((char*[]){ESP_INIT_COMMAND}, 1);

  // Connect to the backend
  uart_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){SERVER_IP}, 1);
  char* yieldedMessage = uart_wait_for_messages((char*[]){ESP_READY_COMMAND, ESP_CLOSE_COMMAND}, 2);

  if (strcmp(yieldedMessage, ESP_CLOSE_COMMAND) == 0) {
    return false;
  }

  return true;
}

void run(int argc, char** argv) {
  if (DEBUG) {
    printf("Running ESP\n");
  }

  char recvBuffer[UART_BUFFER_SIZE];
  char sendBuffer[UART_BUFFER_SIZE];
  // char *sendPtr;
  // char *recvPtr;
  unsigned int count = 0;
  unsigned int failCount = 0;
  bool connected = false;
  struct Pose pose;

  do {
    connected = init_connection();
    failCount += !connected;

    if (!connected && failCount > 0) {
      printf("Failed to connect to backend %d/10 times. Retrying ...\n", failCount);
    }
  } while (!connected && failCount < 10);

  if (!connected) {
    // Add some sorta loop counter here
    printf("ESP Failed to connect to backend. Quitting ...\n");
    return;
  }

  printf("Connected to backend\n");
  // while (strcmp(recvBuffer, "connected") != 0) {
  //   int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
  //   //   recvPtr = recvBuffer;
  //   //   while (*recvPtr != '\n') {
  //   //     uart_read_data((unsigned int *)recvPtr);

  //   //     if (*recvPtr == '\n') {
  //   //       break;
  //   //     }
  //   //     recvPtr++;
  //   //     }
  //   //     *recvPtr = '\0';
  //   printf("[%d] %s\n", len, recvBuffer);
  // }

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

    unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);

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
    unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
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
