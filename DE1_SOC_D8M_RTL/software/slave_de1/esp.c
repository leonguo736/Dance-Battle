#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uart.h"
#include "camera.h"
#include "commands.h"
#include "const.h"
#include "esp.h"

#define SERVER_IP "192.168.48.227:8080"

struct Pose {
  double beat;
  double hourAngle;
  double minuteAngle;
};

void reset_esp(void);
bool init_connection(char* serverIP);

// Function Definitions
void reset_esp() {
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

bool esp_init(int argc, char** argv) {
  #ifdef DEBUG
    printf("ESP Init\n");
  #endif
  
  reset_esp();

  unsigned int failCount = 0;
  bool connected = false;

  do {
    connected = init_connection(argc > 1 ? argv[1] : SERVER_IP);
    failCount += !connected;

    if (!connected && failCount > 0) {
      printf("Failed to connect to backend %d/10 times. Retrying ...\n",
             failCount);
    }

  } while (!connected && failCount < 10);

  #ifdef DEBUG
  if (!connected) {
    printf("ESP Failed to connect to backend. Quitting ...\n");
  } else {
    printf("Connected to backend\n");
  }
  #endif

  return connected;
}

void esp_run(void) {
  #ifdef DEBUG
    printf("Running ESP\n");
  #endif

  char recvBuffer[UART_BUFFER_SIZE];
  char sendBuffer[UART_BUFFER_SIZE];
  unsigned int count = 0;
  struct Pose pose;

  uart_send_command(ESP_TYPE_COMMAND, (char*[]){"ca"}, 1);

  while (count < 10) {

    pose.beat = (rand() % 1000) / (rand() % 100 + 1.0);
    pose.hourAngle = (rand() % 1000) / (rand() % 100 + 1.0);
    pose.minuteAngle = (rand() % 1000) / (rand() % 100 + 1.0);

    sprintf(sendBuffer, "%f %f %f", pose.beat, pose.hourAngle,
            pose.minuteAngle);

    uart_send_command(ESP_POSE_COMMAND, (char*[]){sendBuffer}, 1);

    unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);

    len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);

    len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);

    len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);

    count++;

    usleep(1000000);
  }

  while (1) {
    unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
    printf("[%d] %s\n", len, recvBuffer);

    if (strcmp(recvBuffer, "test")) {
      for (uint8_t i = 0; i < 10; i++) {
        writeDeviceNumber(i);
        usleep(1000000);
      }
    }
  }
}
