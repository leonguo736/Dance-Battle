#include "esp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "const.h"
#include "uart.h"

#define SERVER_IP "192.168.48.227:8080"

/* Private Function Prototypes */
/*
 * Resets the ESP
 */
void reset_esp(void);

/* 
 * Initializes the connection to the backend
 * serverIP - IP address of the backend
 * Returns true if the connection was successful
 */
bool init_connection(char* serverIP);

/*
 * Sends an ESP command to the ESP8266
 * cmd - command to send
 * args - arguments to send
 */
void esp_send_command(char* cmd, char** args, unsigned int numArgs);

/* Private Function Definitions */
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

/* Public Function Definitions */
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

char* esp_read(unsigned int* len) {
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
