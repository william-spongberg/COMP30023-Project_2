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
    if (verify_login(*buffer) == -1) {
        perror("login");
        fprintf(stderr, "Login failure\n");
        exit(3);
    }

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(login);
}

void logout(char **tag, char **buffer, int connfd, FILE *stream) {
    // logout command
    char *logout = create_command(1, LOGOUT);
    send_command(logout, tag, buffer, connfd, stream);

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(logout);
}

void select_folder(char **str_message_num, char *folder, char **tag,
                   char **buffer, int connfd, FILE *stream) {
    char *q_folder = NULL;
    // if folder is NULL, use default INBOX
    if (folder == NULL) {
        folder = INBOX;
    }

    // put quotes around folder name
    q_folder = malloc(strlen(folder) + 3);
    check_memory(q_folder);
    sprintf(q_folder, "\"%s\"", folder);

    // select command
    char *select = create_command(2, SELECT, q_folder);
    send_command(select, tag, buffer, connfd, stream);

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
            start += 2;

            char *end = start;
            while (*end != ' ') {
                end++;
            }

            strncpy(*str_message_num, start, end - start);
            (*str_message_num)[end - start] = '\0';
            fprintf(stderr, "New message number: %s\n", *str_message_num);
        }
    }

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(q_folder);
    free(select);
}

int verify_login(char *buffer) {
    if (strstr(buffer, "Logged in") == NULL) {
        return -1;
    }
    return 0;
}