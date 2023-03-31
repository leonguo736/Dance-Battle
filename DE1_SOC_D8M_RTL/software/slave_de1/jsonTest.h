#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

int jsonTest() {
    char input[] = "echo,3,hello,world,!";
    int input_len = strlen(input);
    char** argv;
    char* cmd;
    int argc = arrayToArgs(input, input_len, &argv, &cmd);

    printf("command = %s\n", cmd);
    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("arg%d = %s\n", i, argv[i]);
    }

    // Free the dynamically allocated memory
    free(cmd);
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);

    return 0;
}