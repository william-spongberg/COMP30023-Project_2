#include "list.h"

#define LIST_CMD "FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n"
#define LIST_CMD_LEN strlen(LIST_CMD)
#define MAX_DATA_SIZE 4096

#define LIST_TAG "A03"

// Helper function to check if the mailbox is empty
int is_empty(const int *client_socket_fd, char *folder_name) {
    // Command should be "STATUS <folder_name> (MESSAGES)"
    char *cmd = malloc(strlen("STATUS ") + strlen(folder_name) + 
                            strlen(" (MESSAGES)\r\n") + 1);
    if (cmd == NULL) {
        perror("malloc");
        return -1;
    }
    strcpy(cmd, "STATUS ");
    strcat(cmd, folder_name);
    strcat(cmd, " (MESSAGES)\r\n");

    // Send the command to the server
    if (send(*client_socket_fd, cmd, strlen(cmd), 0) == -1) {
        perror("send");
        fprintf(stderr, "Failed to send command to server\n");
        free(cmd);
        return -1;
    }

    // Receive the response from the server
    char content[MAX_DATA_SIZE];
    int num_bytes = recv(*client_socket_fd, content, MAX_DATA_SIZE, 0);
    if (num_bytes == -1) {
        perror("recv");
        fprintf(stderr, "Failed to receive response from server\n");
        free(cmd);
        return -1;
    }

    if (num_bytes == 0) {
        fprintf(stderr, "Server closed connection\n");
        free(cmd);
        return -1;
    }

    // Find the number of messages in the mailbox
    char *num_msg = strstr(content, "MESSAGES");
    if (num_msg == NULL) {
        fprintf(stderr, "Failed to find number of messages\n");
        free(cmd);
        return -1;
    }

    num_msg += strlen("MESSAGES") + 1;
    int num_messages = atoi(*num_msg);
    free(cmd);
    if (num_messages == 0) {
        return 1;
    }

    return 0;

}

int list_emails(const int *client_socket_fd, char *folder_name) {
    // Command should be "FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])"
    char *cmd = malloc(LIST_CMD_LEN + 1);
    if (cmd == NULL) {
        perror("malloc");
        return -1;
    }
    strcpy(cmd, LIST_CMD);

    // Check if the mailbox is empty
    if (is_empty(client_socket_fd, folder_name) == 1) {
        printf("");
        free(cmd);
        return 1;
    }


    // Send the command to the server
    if (send(*client_socket_fd, cmd, LIST_CMD_LEN, 0) == -1) {
        perror("send");
        fprintf(stderr, "Failed to send command to server\n");
        free(cmd);
        return -1;
    }

    // Receive the response from the server
    char content[MAX_DATA_SIZE];
    int num_bytes = recv(*client_socket_fd, content, MAX_DATA_SIZE, 0);
    if (num_bytes == -1) {
        perror("recv");
        fprintf(stderr, "Failed to receive response from server\n");
        free(cmd);
        return -1;
    }

    if (num_bytes == 0) {
        fprintf(stderr, "Server closed connection\n");
        free(cmd);
        return -1;
    }

    // Print the response from the server to stdout
    content[num_bytes] = '\0';
    printf("%s\n", content);

    return 0;

}