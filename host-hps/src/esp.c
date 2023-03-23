#include "esp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "commands.h"
#include "const.h"
#include "hex.h"

//#define SERVER_IP "192.168.0.198:8080"

struct Pose {
  double beat;
  double hourAngle;
  double minuteAngle;
};

void esp_reset(void);
bool init_connection(char* serverIP);

// Function Definitions
void esp_reset() {
  // Reset the ESP
  uart_send_command(ESP_RESET_COMMAND, NULL, 0);
  uart_wait_for_messages((char*[]){ESP_INIT_COMMAND}, 1);
}

bool init_connection(char* serverIP) {
  // Connect to the backend
  uart_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){serverIP}, 1);
  char* yieldedMessage = uart_wait_for_messages((char*[]){ESP_READY_COMMAND, ESP_CLOSE_COMMAND}, 2);

  if (strcmp(yieldedMessage, ESP_CLOSE_COMMAND) == 0) {
    return false;
  }

  return true;
}

void esp_run(void* vargp) {
  char** argv = (char**) vargp;
  if (DEBUG) {
    printf("Running ESP\n");
  }

  char recvBuffer[UART_BUFFER_SIZE];
  char sendBuffer[UART_BUFFER_SIZE];

  unsigned int count = 0;
  unsigned int failCount = 0;
  bool connected = false;
  struct Pose pose;

  esp_reset();
  do {
    connected = init_connection(argv[1] != NULL ? argv[1] : SERVER_IP);
    failCount += !connected;

    if (!connected && failCount > 0) {
      printf("Failed to connect to backend %d/10 times. Retrying ...\n", failCount);
    }
  } while (!connected && failCount < 10);

  if (!connected) {
    printf("ESP Failed to connect to backend. Quitting ...\n");
    return;
  }

  printf("Connected to backend\n");

  while (count < 10) {
    // display(count);

    pose.beat = (rand() % 1000) / (rand() % 100 + 1.0);
    pose.hourAngle = (rand() % 1000) / (rand() % 100 + 1.0);
    pose.minuteAngle = (rand() % 1000) / (rand() % 100 + 1.0);

    sprintf(sendBuffer, "%f %f %f", pose.beat, pose.hourAngle,
            pose.minuteAngle);

    uart_send_command(ESP_POSE_COMMAND, (char*[]){sendBuffer}, 1);

    unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);

    count++;
  }

  while (1) {
    unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);
  }

  pthread_exit(NULL);
}
