#include "retrieve.h"
#define MAX_DATA_SIZE 4096


char *construct_retrieve_cmd(const int *msg_num) {

    char msg_num_str[10]; // Assume maximum number of email is 10 digits
    sprintf(msg_num_str, "%d", *msg_num);
    
    int msg_size = RETRIEVE_TAG_SIZE + SPACE + RETRIEVE_CMD_SIZE + SPACE + 
                    strlen(msg_num_str) + SPACE + BOBY_PEEK_SIZE + EOL_SIZE;
    
    

    char *msg = (char *)malloc(msg_size);
    msg = memcpy(msg, RETRIEVE_TAG, RETRIEVE_TAG_SIZE);
    strcat(msg, " ");
    strcat(msg, RETRIEVE_CMD);
    strcat(msg, " ");
    strcat(msg, msg_num_str);
    strcat(msg, " ");
    strcat(msg, BOBY_PEEK);
    strcat(msg, "\r\n");
    return msg;
}



int retrieve_email(const int *client_socket_fd, int *msg_num) {
    if (msg_num == NULL) {
        // Retrieve the last email added

    }

    char *retrieve_cmd = construct_retrieve_cmd(msg_num);

    int byte_send = send(*client_socket_fd, retrieve_cmd, strlen(retrieve_cmd), 0);

    // Check if the send is successful
    if (byte_send == -1) {
        perror("send");
        fprintf(stderr, "Failed to send retrieve command\n");
        return -1;
    }

    if (byte_send == 0) {
        fprintf(stderr, "Connection closed\n");
        return -1;
    }

    free(retrieve_cmd);

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

    // Print the response to stdout
    printf("%s\n", response);

    return 0;
}
