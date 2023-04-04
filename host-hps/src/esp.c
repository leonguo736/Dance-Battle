#include "esp.h"

#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "const.h"

#define SERVER_IP "192.168.48.227:8080"

bool esp_connected = false;

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
  char* yieldedMessage = uart_wait_for_messages(
      (char*[]){ESP_READY_COMMAND, ESP_CLOSE_COMMAND}, 2);

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

  do {
    esp_connected = init_connection(argc > 1 ? argv[1] : SERVER_IP);
    failCount += !esp_connected;

    if (!esp_connected && failCount > 0) {
      printf("Failed to connect to backend %d/10 times. Retrying ...\n",
             failCount);
    }

  } while (!esp_connected && failCount < 10);


  if (!esp_connected) {
    printf("ESP Failed to connect to backend. Quitting ...\n");
  } else {
    uart_send_command(ESP_TYPE_COMMAND, (char*[]){"h"}, 1);
#ifdef DEBUG
    printf("esp_connected to backend\n");
#endif
  }

  return esp_connected;
}

char* esp_read(unsigned int* len) {
#ifdef DEBUG_CRITICAL
  printf("Reading ESP\n");
#endif

  char recvBuffer[UART_BUFFER_SIZE];
  *len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);

  if (*len > 0) {
    char* returnBuffer = (char*)malloc(sizeof(char) * *len);
    memcpy(returnBuffer, recvBuffer, *len);

    return returnBuffer;
  } else {
    return NULL;
  }
}

void esp_write(char* data) {
  uart_send_command(ESP_JSON_COMMAND, (char*[]){data, "\n"}, 2);
}

void esp_run(void) {
#ifdef DEBUG_CRITICAL
  printf("Running ESP\n");
#endif

  // char recvBuffer[UART_BUFFER_SIZE];
  // char sendBuffer[UART_BUFFER_SIZE];


  // while (1) {
  //   unsigned int len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);
  //   printf("[%d] %s\n", len, recvBuffer);
  // }
}
