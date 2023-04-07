#include "esp.h"

#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "const.h"

#define SERVER_IP "192.168.48.227:8080"

bool esp_ready = false;
bool esp_failed = false;

bool init_connection(char* serverIP);
void esp_reset(void);
void esp_write_data(char* data);
int esp_read_data(char* data, int len);
char* esp_wait_for_messages(char** messages, unsigned int numMessages);
void esp_send_command(char* cmd, char** args, unsigned int numArgs);

// Function Definitions
void esp_reset() {
  // Reset the ESP
  esp_send_command(ESP_RESET_COMMAND, NULL, 0);
  esp_wait_for_messages((char*[]){ESP_INIT_COMMAND}, 1);
}

bool init_connection(char* serverIP) {
  // Connect to the backend
  esp_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){serverIP}, 1);
  char* yieldedMessage = esp_wait_for_messages(
      (char*[]){ESP_READY_COMMAND, ESP_CLOSE_COMMAND}, 2);

  if (strcmp(yieldedMessage, ESP_CLOSE_COMMAND) == 0) {
    return false;
  }

  return true;
}

// void* esp_init(void* argp) {
bool esp_init(char* server_ip) {
#ifdef DEBUG
  printf("ESP Init - %s\n",
         server_ip != NULL ? server_ip : SERVER_IP);
#endif
  esp_reset();

  unsigned int failCount = 0;
  bool esp_connected = false;

  do {
    esp_connected =
        init_connection(server_ip != NULL ? server_ip : SERVER_IP);
    failCount += !esp_connected;

    if (!esp_connected && failCount > 0) {
      printf("Failed to connect to backend %d/10 times. Retrying ...\n",
             failCount);
    }

  } while (!esp_connected && failCount < ESP_CONNECT_TIMEOUT);


  if (!esp_connected) {
    printf("ESP Failed to connect to backend. Quitting ...\n");
    esp_failed = true;
  } else {
    esp_send_command(ESP_TYPE_COMMAND, (char*[]){"h"}, 1);
    esp_ready = true;
#ifdef DEBUG
    printf("esp_connected to backend\n");
#endif
  }

  return esp_connected;
}

void esp_write_data(char* str) {
  while (*str != '\0') {
    uart_write_byte((uint16_t)*str);
    str++;
  }
  uart_write_byte('\r');
}

int esp_read_data(char* str, int len) {
  unsigned int actualLen = 0;

  if (uart_read_ready()) {  // Clear buffer
    for (unsigned int i = 0; i < len; i++) {
      str[i] = '\0';
    }

    // Read from UART and store in buffer
    while (*str != '\n' && actualLen < len) {
      uart_read_byte((uint8_t*)str);
      str += *str != '\n';
      actualLen += *str != '\n';
    }

    *str = '\0';

    if (actualLen >= len) {
      printf("Buffer Overflow - %d\n", actualLen);
      actualLen = -1;
    }
  }

  return actualLen;
}

char* esp_wait_for_messages(char** messages, unsigned int numMessages) {
#ifdef DEBUG
  printf("Waiting for messages: ");
  for (unsigned int i = 0; i < numMessages; i++) {
    printf("%s, ", messages[i]);
  }
  printf("\n");
#endif
  char recvBuffer[ESP_BUFFER_SIZE];
  char* yieldedMessage = NULL;
  do {
    esp_read_data(recvBuffer, ESP_BUFFER_SIZE);
    for (unsigned int i = 0; i < numMessages && !yieldedMessage; i++) {
      yieldedMessage =
          strcmp(recvBuffer, messages[i]) == 0 ? messages[i] : NULL;
    }
  } while (!yieldedMessage);
  return yieldedMessage;
}

void esp_send_command(char* cmd, char** args, unsigned int numArgs) {
  char sendBuffer[ESP_BUFFER_SIZE];
#ifdef DEBUG
  printf("Parsing Command: %s |", cmd);
#endif
  sprintf(sendBuffer, "%s", cmd);
  for (unsigned int i = 0; i < numArgs; i++) {
#ifdef DEBUG
    printf(" %s,", args[i]);
#endif
    sprintf(sendBuffer, "%s%s ", sendBuffer, args[i]);
  }

#ifdef DEBUG
  printf("\nSending Command: %s\n", sendBuffer);
#endif
  esp_write_data(sendBuffer);
}

char* esp_read(unsigned int* len) {
#ifdef DEBUG_CRITICAL
  printf("Reading ESP\n");
#endif

  char recvBuffer[ESP_BUFFER_SIZE];
  *len = esp_read_data(recvBuffer, ESP_BUFFER_SIZE);

  if (*len > 0) {
    char* returnBuffer = (char*)malloc(sizeof(char) * *len);
    memcpy(returnBuffer, recvBuffer, *len);

    return returnBuffer;
  } else {
    return NULL;
  }
}

void esp_write(char* data) {
  esp_send_command(ESP_JSON_COMMAND, (char*[]){data, "\n"}, 2);
}