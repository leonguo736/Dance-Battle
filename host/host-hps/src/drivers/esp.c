#include "esp.h"

#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "const.h"

#define SERVER_IP "192.168.48.227:8080"

/* Global Variables */
bool esp_ready = false;
bool esp_failed = false;
bool esp_initialized = false;
unsigned int fail_count = 0;

/* Function Prototypes */
bool init_connection(char* serverIP);
void esp_write_data(char* data);
int esp_read_data(char* data, int len);
char* esp_wait_for_messages(char** messages, unsigned int numMessages);
void esp_send_command(char* cmd, char** args, unsigned int numArgs);

/* Function Definitions */
void esp_reset() {
  // Reset the ESP
  esp_send_command(ESP_RESET_COMMAND, NULL, 0);
  // esp_wait_for_messages((char*[]){ESP_INIT_COMMAND}, 1);
}

bool init_connection(char* serverIP) {
  // Connect to the backend
  esp_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){serverIP}, 1);
  char* yieldedMessage = esp_wait_for_messages(
      (char*[]){ESP_READY_COMMAND, ESP_FAILED_COMMAND}, 2);

  if (strcmp(yieldedMessage, ESP_FAILED_COMMAND) == 0) {
    return false;
  }

  return true;
}

void esp_init(char* server_ip) {
  unsigned int len = 0;
  char* data = esp_read(&len);

  if (data != NULL) {
    server_ip = server_ip != NULL ? server_ip : SERVER_IP;
#ifdef DEBUG_CRITICAL
    printf("ESP Init - %s | %s\n", server_ip != NULL ? server_ip : SERVER_IP,
           data);
#endif
    if (!esp_initialized && strcmp(data, ESP_INIT_COMMAND) == 0) {
      esp_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){server_ip}, 1);
      esp_initialized = true;
    } else if (strcmp(data, ESP_READY_COMMAND) == 0) {
#ifdef DEBUG
      printf("ESP connected to backend @ %s\n", server_ip);
#endif
      esp_ready = true;
    } else if (strcmp(data, ESP_FAILED_COMMAND) == 0) {
      fail_count++;
      printf("Failed to connect to backend %d/%d times. Retrying ...\n",
             fail_count, ESP_CONNECT_THRESHOLD);

      esp_failed = fail_count >= ESP_CONNECT_THRESHOLD;

      esp_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){server_ip}, 1);
    }
    free(data);
  }
}

// void* esp_init(void* argp) {
// bool esp_init(char* server_ip) {
// #ifdef DEBUG
//   printf("ESP Init - %s\n",
//          server_ip != NULL ? server_ip : SERVER_IP);
// #endif
//   esp_reset();

//   unsigned int failCount = 0;
//   bool esp_connected = false;

//   do {
//     esp_connected =
//         init_connection(server_ip != NULL ? server_ip : SERVER_IP);
//     failCount += !esp_connected;

//     if (!esp_connected && failCount > 0) {
//       printf("Failed to connect to backend %d/10 times. Retrying ...\n",
//              failCount);
//     }

//   } while (!esp_connected && failCount < ESP_CONNECT_TIMEOUT);

//   if (!esp_connected) {
//     printf("ESP Failed to connect to backend. Quitting ...\n");
//     esp_failed = true;
//   } else {
//     esp_send_command(ESP_TYPE_COMMAND, (char*[]){"h"}, 1);
//     esp_ready = true;
// #ifdef DEBUG
//     printf("esp_connected to backend\n");
// #endif
//   }

//   return esp_connected;
// }

void esp_write_data(char* str) {
  while (*str != '\0') {
    uart_write_byte((uint16_t)*str);
    str++;
  }
  uart_write_byte('\r');
}

int esp_read_data(char* str, int max_len) {
  unsigned int read_len = 0;

  if (uart_read_ready()) {  
    // Clear buffer
    for (unsigned int i = 0; i < max_len; i++) {
      str[i] = '\0';
    }

    // Read from UART and store in buffer
    while (*str != '\n' && read_len < max_len) {
      if (uart_read_byte((uint8_t*)str)) {
        str += *str != '\n';
        read_len += *str != '\n';
      } else {
        return -1;
      }
    }

    *str = '\0';
    read_len++;

    if (read_len >= max_len) {
      printf("Buffer Overflow - %d\n", read_len);
      read_len = -1;
    }
  }

  return read_len;
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
#ifdef DEBUG_CRITICAL
    printf("ESP_Read [%d]-----\n%s\n-----\n%s\n-----\n", *len, recvBuffer, returnBuffer);
#endif
    return returnBuffer;
  } else {
    return NULL;
  }
}

void esp_write(char* data) {
  esp_send_command(ESP_JSON_COMMAND, (char*[]){data, "\n"}, 2);
}

void esp_close(char* server_ip) { 
  printf("Closing ESP\n");

  fail_count = 0;
  esp_ready = false;
  esp_failed = false;

  // Attempt to reconnect
  esp_send_command(ESP_CONNECT_BACKEND_COMMAND, (char*[]){server_ip}, 1);
}