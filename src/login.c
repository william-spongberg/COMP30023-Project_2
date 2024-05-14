#include "login.h"
#include "command.h"
#include "memory.h"
#include "tag.h"

void login(char *username, char *password, char **tag, char **buffer,
           int connfd, FILE *stream) {
    // login command
    char *login = create_command(3, LOGIN, username, password);
    send_command(login, tag, buffer, connfd, stream);

    // verify that login was successful
    if (verify_login(*tag, *buffer) == -1) {
        perror("login");
        fprintf(stderr, "Login failure\n");
        exit(3);
    }

    // print response for debugging
    // printf("Received:\n%s\n", *buffer);
    // printf("\n");

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(login);
}

void logout(char **tag, char **buffer, int connfd, FILE *stream) {
    // logout command
    char *logout = create_command(1, LOGOUT);
    send_command(logout, tag, buffer, connfd, stream);

    // print response for debugging
    // printf("Received:\n%s\n", *buffer);
    // printf("\n");

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(logout);
}

void select_folder(char **str_message_num, char *folder, char **tag,
                   char **buffer, int connfd, FILE *stream) {
    char *q_folder = NULL;
    if (folder == NULL) {
        // default folder
        folder = INBOX;
    }

    // put quotes around folder name
    q_folder = malloc(strlen(folder) + 3);
    check_memory(q_folder);
    sprintf(q_folder, "\"%s\"", folder);

    // select command
    char *select = create_command(2, SELECT, q_folder);
    send_command(select, tag, buffer, connfd, stream);

    // verify that folder selection was successful
    // if (verify_folder_selection(*buffer, NULL) == -1) {
    //     perror("select");
    //     fprintf(stderr, "Folder not found\n");
    //     // TODO: exit on failure?
    //     exit(3);
    // }

    if (*str_message_num == NULL) {
        // get number of messages in folder
        char *start = strstr(*buffer, "EXISTS");
        if (start == NULL) {
            perror("select");
            fprintf(stderr, "Failed to find number of messages\n");
        } else {
            while (*start != '*') {
                start--;
            }
            // skip over "* "
            start++;
            start++;

            char *end = start;
            while (*end != ' ') {
                end++;
            }

            strncpy(*str_message_num, start, end - start);
            (*str_message_num)[end - start] = '\0';
            fprintf(stderr, "New message number: %s\n", *str_message_num);
        }
    }

    // print response for debugging
    // printf("Received:\n%s\n", *buffer);
    // printf("\n");

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(q_folder);
    free(select);
}

int verify_login(char *tag, char *buffer) {
    // TODO: fix this + mem leaks caused by it
    // If the response does not contain "Logged in", then it is a fatal response
    if (strstr(buffer, "Logged in") == NULL) {
        return -1;
    }

    return 0;
}

int verify_folder_selection(char *buffer, int *msg_num) {
    // TODO: fix this + mem leaks caused by it
    // if msg_num from the command is not found, it is assigned to the last
    // email added, which is the number of email in the mailbox, which is
    // received here
    // if (msg_num == NULL) {
    //     char *num_msg = strstr(buffer, "EXISTS");
    //     if (num_msg == NULL) {
    //         perror("select");
    //         fprintf(stderr, "Failed to find number of messages\n");
    //         // TODO: return different error code, treat differently?
    //         return -1;
    //     }

    //     num_msg += strlen("EXISTS") + 1;
    //     *msg_num = atoi(num_msg);
    // }

    return 0;
}
