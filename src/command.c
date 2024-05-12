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
    // initialise commands

    // total command
    get_num_tag(*tag, MAX_TAG_SIZE);
    char *total_command = (char *)malloc(strlen(*tag) + strlen(command) + 2);
    check_memory(total_command);
    strcpy(total_command, *tag);
    strcat(total_command, " ");
    strcat(total_command, command);
    // ok command
    char *ok_command = malloc(strlen(*tag) + 4);
    check_memory(ok_command);
    strcpy(ok_command, *tag);
    strcat(ok_command, " OK");
    // no command
    char *no_command = malloc(strlen(*tag) + 4);
    check_memory(no_command);
    strcpy(no_command, *tag);
    strcat(no_command, " NO");
    // bad command
    char *bad_command = malloc(strlen(*tag) + 5);
    check_memory(bad_command);
    strcpy(bad_command, *tag);
    strcat(bad_command, " BAD");

    // send command to server
    write(connfd, total_command, strlen(total_command));
    // printf("Sent:\n%s\n", total_command);

    // receive first response from server
    char line[MAX_LINE_SIZE];
    fgets(line, MAX_LINE_SIZE, stream);
    strcat(*buffer, line);

    // check if command was successful
    if (strncmp(*buffer, no_command, strlen(no_command)) == 0 ||
        strncmp(*buffer, bad_command, strlen(bad_command)) == 0) {
        if (strstr(command, "FETCH") != NULL) {
            printf("Message not found\n");
            fprintf(stderr, "Failed to retrieve message\n");
            fprintf(stderr, "Received: %s\n", *buffer);
            exit(3);
        } else if (strstr(command, "LOGIN") != NULL) {
            printf("Login failure\n");
            fprintf(stderr, "Received: %s\n", *buffer);
            exit(3);
        } else if (strstr(command, "SELECT") != NULL) {
            printf("Folder not found\n");
            fprintf(stderr, "Received: %s\n", *buffer);
            exit(3);
        }

        fprintf(stderr, "Failed to send command\n");
        fprintf(stderr, "Received: %s\n", *buffer);
        exit(3);
    }

    // check if server is ready
    if (strncmp(*buffer, "* ", 2) != 0) {
        fprintf(stderr, "Server not ready\n");
        fprintf(stderr, "Received: %s\n", *buffer);
        exit(3);
    }

    // receive response from server
    while (strncmp(line, ok_command, strlen(ok_command)) != 0 &&
           strncmp(line, no_command, strlen(no_command)) != 0 &&
           strncmp(line, bad_command, strlen(bad_command)) != 0) {
        fgets(line, MAX_LINE_SIZE, stream);
        // realloc buffer if needed
        if (strlen(*buffer) + strlen(line) >= MAX_DATA_SIZE) {
            *buffer = realloc(*buffer, strlen(*buffer) * REALLOC_SIZE + 1);
            check_memory(*buffer);
        }
        // DO NOT cat buffer if line is the status of the command
        strcat(*buffer, line);

        // printf("%s", line);
    }

    // check if task was successful
    if (strncmp(line, no_command, strlen(no_command)) == 0 ||
        strncmp(line, bad_command, strlen(bad_command)) == 0) {

        if (strstr(command, "FETCH") != NULL) {
            fprintf(stderr, "Failed to retrieve message\n");
            fprintf(stderr, "Received: %s\n", *buffer);
            exit(3);
        } else if (strstr(command, "LOGIN") != NULL) {
            printf("Login failure\n");
            fprintf(stderr, "Received: %s\n", *buffer);
            exit(3);
        } else if (strstr(command, "SELECT") != NULL) {
            printf("Folder not found\n");
            fprintf(stderr, "Received: %s\n", *buffer);
            exit(3);
        }
    }

    // free memory
    free(total_command);
    free(ok_command);
    free(no_command);
    free(bad_command);
}