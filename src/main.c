#include "login.h"
#include "parse.h"
#include "read.h"
#include "tag.h"
#include "command.h"
#include "memory.h"
#include "connect.h"
#include "mime.h"

#define LOGIN "LOGIN"
#define SELECT "SELECT"
#define FETCH "FETCH"
#define BODY "BODY.PEEK[]"
#define BODY_HEADERS "BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]"

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
    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("Folder: %s\n", folder);
    printf("Message number: %d\n", int_message_num);
    printf("Command: %s\n", command);
    printf("Hostname: %s\n\n", hostname);

    // setup connection
    int connfd = setup_connection(hostname);

    // initialise stream
    FILE *stream = fdopen(connfd, "r+");

    // initialise buffer
    char *buffer = (char *)calloc(MAX_DATA_SIZE, sizeof(char));
    check_memory(buffer);
    char *tag = (char *)calloc(MAX_TAG_SIZE, sizeof(char));
    check_memory(tag);
    get_num_tag(tag, MAX_TAG_SIZE);
    login(&connfd, username, password);
    select_folder(&connfd, folder, &int_message_num);

    /**
     * TODO: Proposed: instead of sending command in main, let each module send
     * its own respective command, and receive the response. This way, the main
     * function will only be responsible for setting up the connection,
     * determine what command is requested by tasks and close the connection.
     * (eliminates the need for a giant if else block.)
     */

    // commands
    if (strcmp(command, "retrieve") == 0) {
        printf("|Retrieve command|\n");
        // retrieve command
        char *body = create_command(3, FETCH, str_message_num, BODY);
        send_command(&tag, body, &buffer, connfd, stream);
        // print response
        printf("Received: %s\n", buffer);
        printf("\n");
        memset(buffer, 0, MAX_DATA_SIZE);
        free(body);
    } else if (strcmp(command, "parse") == 0) {
        printf("|Parse command|\n");
        // parse command
        char *headers = create_command(3, FETCH, str_message_num, BODY_HEADERS);
        send_command(&tag, headers, &buffer, connfd, stream);
        parse_headers(buffer);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(headers);
    } else if (strcmp(command, "mime") == 0) {
        printf("|MIME command|\n");
        // mime command
        char *body = create_command(3, FETCH, str_message_num, BODY);
        send_command(&tag, body, &buffer, connfd, stream);
        // TODO: fix args given to get_mime
        //get_mime(buffer);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(body);
    } else if (strcmp(command, "list") == 0) {
        printf("|List command|\n");
        // TODO: fix list command not returning anything
        // list command
        char *list = create_command(1, "UID FETCH 1:* (UID)");
        send_command(&tag, list, &buffer, connfd, stream);
        // print response
        printf("Received: %s\n", buffer);
        printf("\n");
        memset(buffer, 0, MAX_DATA_SIZE);
        free(list);
    } else {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // free memory
    free_memory(connfd, stream, 2, tag, buffer);

    return 0;
}
