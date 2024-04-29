//
// Created by liang on 4/27/2024.
//

#include "login.h"
#include <string.h>

#define LOGIN_TAG "A01"
#define TAG_SIZE 3
#define LOGIN_COMMAND "LOGIN"
#define LOGIN_COMMAND_SIZE 5
#define SPACE_COUNT 3
#define EOL_SIZE 2

// Helper function to construct message in appropriate format
char *construct_message(char *username, char *password) {
    // Allocate memory for the login command
    int username_length = strlen(username);
    int password_length = strlen(password);
    char *login_command =
        (char *)malloc(TAG_SIZE + LOGIN_COMMAND_SIZE + username_length +
                       password_length + SPACE_COUNT + EOL_SIZE + 1);
    if (login_command == NULL) {
        perror("malloc");
        exit(1);
    }

    // Construct the login command
    login_command = memcpy(login_command, LOGIN_TAG, TAG_SIZE);
    strcat(login_command, " ");
    strcat(login_command, LOGIN_COMMAND);
    strcat(login_command, username);
    strcat(login_command, " ");
    strcat(login_command, password);
    strcat(login_command, "\r\n");

    return login_command;
}

// Implement the logon function
int login(const int *client_socket_fd, char *username, char *password) {
    char *login_command = construct_message(username, password);

    // Send the login command to the server already connected via TCP
    int byte_send =
        send(*client_socket_fd, login_command, strlen(login_command), 0);
    if (byte_send == -1) {
        perror("send");
        return -1;
    }
    // Free the memory allocated for the logon command
    free(login_command);
    return 0;
}
