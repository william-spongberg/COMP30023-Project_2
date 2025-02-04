#include "command.h"
#include "connect.h"
#include "input.h"
#include "list.h"
#include "login.h"
#include "memory.h"
#include "mime.h"
#include "parse.h"
#include "retrieve.h"
#include "tag.h"

#define FETCH "FETCH"

int main(int argc, char *argv[]) {
    char *username = NULL;
    char *password = NULL;
    char *folder = NULL;
    char *str_message_num = malloc(MAX_MESSAGE_NUM_SIZE * sizeof(char));
    check_memory(str_message_num);
    char *command = NULL;
    char *hostname = NULL;

    // read command line arguments
    read_command_line(argc, argv, &username, &password, &folder,
                      &str_message_num, &command, &hostname);

    // print command line arguments for debugging
    // printf("\nUsername: %s\n", username);
    // printf("Password: %s\n", password);
    // printf("Folder: %s\n", folder);
    // printf("Message number: %s\n", str_message_num);
    // printf("Command: %s\n", command);
    // printf("Hostname: %s\n\n", hostname);

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

    // login
    login(username, password, &tag, &buffer, connfd, stream);

    // select folder
    select_folder(&str_message_num, folder, &tag, &buffer, connfd, stream);

    /**
     * TODO: Proposed: instead of sending command in main, let each module send
     * its own respective command, and receive the response. This way, the main
     * function will only be responsible for setting up the connection,
     * determine what command is requested by tasks and close the connection.
     * (eliminates the need for a giant if else block.)
     */

    // commands
    if (strcmp(command, "retrieve") == 0) {
        char *body =
            retrieve_body(str_message_num, &tag, &buffer, connfd, stream);
        printf("%s\r\n", body);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(body);
    } else if (strcmp(command, "parse") == 0) {
        parse_headers(str_message_num, &tag, &buffer, connfd, stream);
        memset(buffer, 0, MAX_DATA_SIZE);
    } else if (strcmp(command, "mime") == 0) {
        char *body = create_command(3, FETCH, str_message_num, "BODY.PEEK[]");
        send_command(body, &tag, &buffer, connfd, stream);
        get_mime(buffer);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(body);
    } else if (strcmp(command, "list") == 0) {
        char *fetch_command =
            create_command(1, "UID FETCH 1:* (BODY[HEADER.FIELDS (SUBJECT)])");
        send_command(fetch_command, &tag, &buffer, connfd, stream);
        parse_list(buffer);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(fetch_command);
    } else {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // logout
    logout(&tag, &buffer, connfd, stream);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }

    // free memory
    free_memory(stream, 3, tag, buffer, str_message_num);

    return 0;
}
