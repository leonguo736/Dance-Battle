#include "esp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "const.h"
#include "uart.h"

#define SERVER_IP "137.82.84.224:8080"

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
  DEBUGPRINT("INFO: Initializing ESP ... "); 

  reset_esp();

  // DEBUGPRINT("DEBUG: here1\n"); 

  unsigned int failCount = 0;
  bool connected = false;

  do {
    // DEBUGPRINT("DEBUG: here2\n", 0);

    connected = init_connection(argc > 1 ? argv[1] : SERVER_IP);

    // DEBUGPRINT("DEBUG: here3\n", 0);

    failCount += !connected;

    if (!connected && failCount > 0) {
      DEBUGPRINT("fail %d ",
             failCount);
    }

  } while (!connected);

  // DEBUGPRINT("DEBUG: here4\n", 0);

  if (connected) {
    DEBUGPRINT("connected\n", 0);    
  } else {
    DEBUGPRINT("failed, quitting\n", 0);
  }

  return connected;
}

char* esp_read(unsigned int* len) {
  char recvBuffer[UART_BUFFER_SIZE];
  *len = uart_read_data(recvBuffer, UART_BUFFER_SIZE);

  if (*len > 0) {
    char* returnBuffer = (char*)malloc(sizeof(char) * *len);
    memcpy(returnBuffer, recvBuffer, *len);
    returnBuffer[*len] = '\0';
    return returnBuffer;
  } else {
    return NULL;
  }
}

void esp_write(char* data) {
  uart_send_command(ESP_JSON_COMMAND, (char*[]){data, "\n"}, 2);
}
