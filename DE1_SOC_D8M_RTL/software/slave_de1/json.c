#include "json.h"

#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

int arrayToArgs(char* input, int input_len, char*** argv_ptr, char** cmd_ptr) {
    // Allocate memory for a copy of the input string
    char* input_copy = (char*) malloc(sizeof(char) * (input_len + 1));
    strncpy(input_copy, input, input_len);
    input_copy[input_len] = '\0';

    char* token = strtok(input_copy, ",");
    *cmd_ptr = (char*) malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(*cmd_ptr, token);

    token = strtok(NULL, ",");
    int argc = atoi(token);

    // Allocate memory for the argv array
    char** argv = (char**) malloc(sizeof(char*) * argc);
    *argv_ptr = argv;

    // Parse the arguments and store them in the argv array
    for (int i = 0; i < argc; i++) {
        token = strtok(NULL, ",");
        argv[i] = (char*) malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(argv[i], token);
    }

    // Free the dynamically allocated memory for the input copy
    free(input_copy);

    return argc;
}
