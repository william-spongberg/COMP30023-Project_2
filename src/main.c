#include "command.h"
#include "connect.h"
#include "login.h"
#include "memory.h"
#include "mime.h"
#include "parse.h"
#include "read.h"
#include "retrieve.h"
#include "tag.h"

#define FETCH "FETCH"

int main(int argc, char *argv[]) {
    char *username = NULL;
    char *password = NULL;
    char *folder = NULL;
    char *str_message_num = NULL;
    int int_message_num = 0;
    char *command = NULL;
    char *hostname = NULL;

    // read command line arguments
    read_command_line(argc, argv, &username, &password, &folder,
                      &str_message_num, &command, &hostname);
    int_message_num = atoi(str_message_num);

    // print command line arguments for debugging
    printf("\nUsername: %s\n", username);
    printf("Password: %s\n", password);
    printf("Folder: %s\n", folder);
    printf("Message number: %d\n", int_message_num);
    printf("Command: %s\n", command);
    printf("Hostname: %s\n\n", hostname);

    // setup connection
    int connfd = setup_connection(hostname);

    // initialise stream
    FILE *stream = fdopen(connfd, "r+");

    // initialise buffer + tag
    char *buffer = (char *)calloc(MAX_DATA_SIZE, sizeof(char));
    check_memory(buffer);
    char *tag = (char *)calloc(MAX_TAG_SIZE, sizeof(char));
    check_memory(tag);
    get_num_tag(tag, MAX_TAG_SIZE);

    // login and select folder
    login(username, password, &tag, &buffer, connfd, stream);
    select_folder(folder, &tag, &buffer, connfd, stream);

    /**
     * TODO: Proposed: instead of sending command in main, let each module send
     * its own respective command, and receive the response. This way, the main
     * function will only be responsible for setting up the connection,
     * determine what command is requested by tasks and close the connection.
     * (eliminates the need for a giant if else block.)
     */

    // commands
    if (strcmp(command, "retrieve") == 0) {
        printf("[retrieve]\n");
        retrieve_body(str_message_num, &tag, &buffer, connfd, stream);
        // print response
        printf("Received:\n%s\n", buffer);
        printf("\n");
        memset(buffer, 0, MAX_DATA_SIZE);
    } else if (strcmp(command, "parse") == 0) {
        printf("[parse]\n");
        parse_headers(str_message_num, &tag, &buffer, connfd, stream);
        memset(buffer, 0, MAX_DATA_SIZE);
    } else if (strcmp(command, "mime") == 0) {
        printf("[mime]\n");
        // TODO: fix mime
        get_mime(str_message_num, &tag, &buffer, connfd, stream);
        memset(buffer, 0, MAX_DATA_SIZE);
    } else if (strcmp(command, "list") == 0) {
        printf("[list]\n");
        // TODO: fix fetch command not returning anything
        // TODO: use list.c methods
        // fetch command
        char *fetch_command = create_command(1, "UID FETCH 1:* (UID)");
        send_command(fetch_command, &tag, &buffer, connfd, stream);
        // print response
        printf("Received:\n%s\n", buffer);
        printf("\n");
        memset(buffer, 0, MAX_DATA_SIZE);
        free(fetch_command);
    } else {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // free memory
    free_memory(connfd, stream, 2, tag, buffer);

    return 0;
}
