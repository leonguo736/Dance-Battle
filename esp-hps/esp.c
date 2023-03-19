#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp.h"
#include "hex.h"

struct Pose {
    double beat;
    double hourAngle;
    double minuteAngle;
};

void run(void) {
    char recvBuffer[1024];
    char sendBuffer[1024];
    char *sendPtr;
    char *recvPtr;
    unsigned int count = 0;
    struct Pose pose;

    while (strcmp(recvBuffer, "connected") != 0) {
        recvPtr = recvBuffer;
        while (*recvPtr != '\n') {
            uart_read_data((unsigned int *)recvPtr);

            if (*recvPtr == '\n') {
                break;
            }
            recvPtr++;
        }
        *recvPtr = '\0';
        printf("%s\n", recvBuffer);
    }

    uart_write_data('s');
    uart_write_data('t');
    uart_write_data('a');
    uart_write_data('r');
    uart_write_data('t');
    uart_write_data('\r');

    while (count < 10) {
        display(count);

        pose.beat = (rand() % 1000) / (rand() % 100 + 1.0);
        pose.hourAngle = (rand() % 1000) / (rand() % 100 + 1.0);
        pose.minuteAngle = (rand() % 1000) / (rand() % 100 + 1.0);

        sprintf(sendBuffer, "%f,%f,%f", pose.beat, pose.hourAngle, pose.minuteAngle);

        sendPtr = sendBuffer;
        while (*sendPtr != '\0') {
            uart_write_data((unsigned int) *sendPtr);
            sendPtr++;
        }

        uart_write_data('\r');

        recvPtr = recvBuffer;
        while (*recvPtr != '\n') {
            uart_read_data((unsigned int *)recvPtr);

            if (*recvPtr == '\n') {
                break;
            }
            recvPtr++;
        }
        *recvPtr = '\0';
        printf("%s\n", recvBuffer);
        count++;
    }

    while (1) {
        recvPtr = recvBuffer;
        while (*recvPtr != '\n') {
            uart_read_data((unsigned int *)recvPtr);

            if (*recvPtr == '\n') {
                break;
            }
            recvPtr++;
        }
        *recvPtr = '\0';
        printf("%s\n", recvBuffer);
    }
}
