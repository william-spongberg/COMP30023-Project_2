#include "command.h"
#include "memory.h"
#include "tag.h"

// TODO: convert magic numbers to defines

char *create_command(int num_strs, ...) {
    va_list args;
    va_start(args, num_strs);

    // calculate total length of all strings
    int total_length = 0;
    for (int i = 0; i < num_strs; i++) {
        char *str = va_arg(args, char *);
        if (str != NULL)
            total_length += strlen(str) + 1;
    }
    va_end(args);

    va_start(args, num_strs);

    // create total command
    char *total_command =
        (char *)malloc(total_length + num_strs + CARRIAGE_RETURN_NEWLINE_SIZE);
    check_memory(total_command);
    total_command[0] = '\0';

    // concatenate all strings
    for (int i = 0; i < num_strs; i++) {
        strcat(total_command, va_arg(args, char *));
        strcat(total_command, " ");
    }

    // add carriage return and newline
    strcat(total_command, CARRIAGE_RETURN_NEWLINE);

    va_end(args);
    return total_command;
}

void send_command(char *command, char **tag, char **buffer, int connfd,
                  FILE *stream) {
    // initialise command
    get_num_tag(*tag, MAX_TAG_SIZE);
    char *total_command = (char *)malloc(strlen(*tag) + strlen(command) + 2);
    strcpy(total_command, *tag);
    strcat(total_command, " ");
    strcat(total_command, command);

    // send command to server
    write(connfd, total_command, strlen(total_command));
    printf("Sent:\n%s\n", total_command);

    // receive first response from server
    char line[MAX_LINE_SIZE];
    fgets(line, MAX_LINE_SIZE, stream);
    strcat(*buffer, line);

    // check if server is ready
    if (strncmp(*buffer, "* ", 2) != 0) {
        fprintf(stderr, "Server not ready\n");
        fprintf(stderr, "Received: %s\n", *buffer);
        exit(3);
    }

    // confirm command
    char *confirm_command = malloc(strlen(*tag) + 4);
    strcpy(confirm_command, *tag);
    strcat(confirm_command, " OK");
    while (strncmp(line, confirm_command, strlen(confirm_command)) != 0) {
        fgets(line, MAX_LINE_SIZE, stream);
        // realloc buffer if needed
        if (strlen(*buffer) + strlen(line) >= MAX_DATA_SIZE) {
            *buffer = realloc(*buffer, strlen(*buffer) * REALLOC_SIZE + 1);
            if (*buffer == NULL) {
                fprintf(stderr, "Failed to allocate memory\n");
                exit(5);
            }
        }
        strcat(*buffer, line);
        // printf("%s", line);
    }

    // free memory
    free(total_command);
    free(confirm_command);
}