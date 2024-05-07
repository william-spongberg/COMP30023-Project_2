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
        printf("Login failed\n");
        exit(3);
    }

    // print response for debugging
    // printf("Received:\n%s\n", *buffer);
    // printf("\n");

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(login);
}

void select_folder(char *folder, char **tag, char **buffer, int connfd,
                   FILE *stream) {
    // put quotes around folder name
    char *quoted_folder = malloc(strlen(folder) + 3);
    check_memory(quoted_folder);
    sprintf(quoted_folder, "\"%s\"", folder);

    // select command
    char *select = create_command(2, SELECT, quoted_folder);
    send_command(select, tag, buffer, connfd, stream);

    // verify that folder selection was successful
    if (verify_folder_selection(*buffer, NULL) == -1) {
        perror("select");
        fprintf(stderr, "Folder not found\n");
        // TODO: exit on failure?
        exit(3);
    }

    // print response for debugging
    // printf("Received:\n%s\n", *buffer);
    // printf("\n");

    // free memory
    memset(*buffer, 0, MAX_DATA_SIZE);
    free(quoted_folder);
    free(select);
}

int verify_login(char *tag, char *buffer) {
    // TODO: fix this + mem leaks caused by it
    // If the response does not contain "Logged in", then it is a fatal response
    if (strstr(buffer, "Logged in") == NULL) {
        perror("login");
        fprintf(stderr, "Failed to login\n");
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
