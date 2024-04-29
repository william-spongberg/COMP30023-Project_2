//
// Created by liang on 4/27/2024.
//

#include "login.h"
#include <string.h>

#define LOGIN_TAG "A01"
#define SELECT_TAG "A02"
#define TAG_SIZE 3
#define LOGIN_COMMAND "LOGIN"
#define LOGIN_COMMAND_SIZE 5
#define SELECT_COMMAND "SELECT"
#define SELECT_COMMAND_SIZE 6
#define SPACE_COUNT 3
#define EOL "\r\n"
#define EOL_SIZE 2
#define DEFAULT_FOLDER "INBOX"
#define MAX_DATA_SIZE 4096

// Helper function to construct message in appropriate format
char *construct_login_message(char *username, char *password) {
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
    char *login_command = construct_login_message(username, password);

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

int verify_login(const int *client_socket_fd) {
    char response[MAX_DATA_SIZE];


    // Read from the server for response
    int recv_len;
    recv_len = recv(*client_socket_fd, response, MAX_DATA_SIZE, 0);
    reponse[recv_len] = '\0'; // Null terminate the received message
    
    if (recv_len == -1) {
        perror("recv");
        fprintf(stderr, "Failed to receive response\n");
        return -1;
    }

    // If the response does not start with the tag, then it is a fatal response
    if (strncmp(response, LOGIN_TAG, TAG_SIZE) != 0) {
        perror("login");
        fprintf(stderr, "Failed to login\n");
        return -1;
    }

    return 0;
}

// Helper function to construct select folder message
char *construct_select_msg(char *folder) {
    int folder_name_size;

    if (folder == NULL) {
        folder_name_size = (int)strlen(DEFAULT_FOLDER);
    } else {
        folder_name_size = (int)strlen(folder);
    }

    char *msg = (char *)malloc(TAG_SIZE + SELECT_COMMAND_SIZE +
                               folder_name_size + EOL_SIZE + 2);

    msg = memcpy(msg, SELECT_TAG, TAG_SIZE);
    strcat(msg, " ");
    strcat(msg, SELECT_COMMAND);
    strcat(msg, " ");
    strcat(msg, folder);
    strcat(msg, EOL);

    return msg;
}

int select_folder(const int *client_socket_fd, char *folder) {
    char *select_cmd = construct_select_msg(folder);

    int byte_send = send(*client_socket_fd, select_cmd, strlen(select_cmd), 0);

    if (byte_send == -1) {
        perror("send");
        fprintf(stderr, "Failed to send select folder command\n");
        return -1;
    }

    free(select_cmd);
    return 0;
}

int verify_folder_selection(const int *client_socket_fd) {
    // Receive response from server
    char response[MAX_DATA_SIZE];

    int recv_len;
    recv_len = recv(*client_socket_fd, response, MAX_DATA_SIZE, 0); // Read all the response
    if (recv_len == -1) {
        perror("recv");
        fprintf(stderr, "Failed to receive response\n");
        return -1;
    }


    if (recv_len == 0) {
        perror("recv");
        fprintf(stderr, "Connection closed\n");
        return -1;
    }

    response[recv_len] = '\0'; // Null terminate the received message

    // Expected response
    char *ok_msg = (char*)malloc(TAG_SIZE + 3 + 1);
    ok_msg[0] = '*';
    ok_msg = strcat(ok_msg, SELECT_TAG);
    ok_msg = strcat(ok_msg, " OK\0");

    // Compare server response to expected response
    if (strncmp(response, ok_msg, strlen(ok_msg)) != 0) {
        perror("select");
        fprintf(stderr, "Folder not found\n");
        return -1;
    } 

    return 0;
}
