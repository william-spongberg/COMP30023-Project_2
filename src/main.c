#include "parse.h"
#include "read.h"
#include "login.h"
#include <netdb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define _POSIX_C_SOURCE 200112L

#define IMAP_PORT "143"
#define LOGIN "LOGIN"
#define SELECT "SELECT"
#define FETCH "FETCH"
#define BODY "BODY.PEEK[]"
#define BODY_HEADERS "BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]"
#define MAX_TAG 10000
#define MAX_TAG_SIZE 4
#define MAX_DATA_SIZE 4096
#define MAX_LINE_SIZE 1024
#define REALLOC_SIZE 2

void get_tag(char *buffer, size_t size);
void send_command(char **tag, char *command, char **buffer, int connfd,
                  FILE *stream);
char *create_command(int num_strs, ...);
int setup_connection(char *hostname);
void check_memory(void *args);
void free_memory(int connfd, FILE *stream, int num_ptrs, ...);
void parse_mime(char *buffer);

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
    get_tag(tag, MAX_TAG_SIZE);


    /**
     * TODO: Proposed: refactor this to the login module
     * which logs in the user and selects the folder.
    */
    // login command
    char *login = create_command(3, LOGIN, username, password);
    send_command(&tag, login, &buffer, connfd, stream);
    // print response
    printf("Received: %s\n", buffer);
    printf("\n");
    memset(buffer, 0, MAX_DATA_SIZE);
    free(login);

    // select command
    char *select = create_command(2, SELECT, folder);
    send_command(&tag, select, &buffer, connfd, stream);
    // print response
    printf("Received: %s\n", buffer);
    printf("\n");
    memset(buffer, 0, MAX_DATA_SIZE);
    free(select);


    /**
     * TODO: Propsoed: the suggestion written for the create_command function
     * below would eliminates the need for a giant if else block.
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
        parse_mime(buffer);
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

/**
 * TODO: Proposed: refactor this to the mime module, 
 * and let the mime module handle the parsing of the mime headers
 * as well as print the contents of the message.
*/
void parse_mime(char *buffer) {
    // TODO: error handling
    // match mime boundary
    char *tmp = strstr(buffer, "boundary=");
    if (tmp == NULL) {
        fprintf(stderr, "No boundary found\n");
        exit(4);
    }

    // get boundary
    char *start = strchr(tmp, '=') + 1;
    char *end = strchr(tmp, '\r');
    tmp = (char *)malloc(end - start + 1);
    strncpy(tmp, start, end - start);
    tmp[end - start] = '\0';

    char *boundary = (char *)malloc(strlen(tmp) + 3);
    strcpy(boundary, "--");
    strcat(boundary, tmp);
    printf("Boundary: %s\n", boundary);

    // get headers using position given by boundary
    char *hdr = strstr(buffer, boundary);
    start = strstr(hdr, "Content-Transfer-Encoding: ");
    start = strchr(start, ' ') + 1;
    end = strchr(start, '\r');
    char *encoding = (char *)malloc(end - start + 1);
    strncpy(encoding, start, end - start);
    encoding[end - start] = '\0';
    printf("Content-Transfer-Encoding: %s\n", encoding);

    // free memory
    free(encoding);
    
    start = strstr(hdr, "Content-Type: ");
    start = strchr(start, ' ') + 1;
    end = strchr(start, '\r');
    encoding = (char *)malloc(end - start + 1);
    strncpy(encoding, start, end - start);
    encoding[end - start] = '\0';
    printf("Content-Type: %s\n", encoding);

    start = strstr(hdr, "\r\n\r\n");
    start += 4;
    end = strstr(start, boundary);
    char *message = (char *)malloc(end - start + 1);
    strncpy(message, start, end - start);
    message[end - start] = '\0';
    printf("[Start Message]%s", message);
    printf("[End Message]\n\n");

    // free memory
    free(tmp);
    free(boundary);
    free(encoding);
    free(message);
}

void free_memory(int connfd, FILE *stream, int num_ptrs, ...) {
    // free memory
    va_list args;
    va_start(args, num_ptrs);
    for (int i = 0; i < num_ptrs; i++) {
        free(va_arg(args, void *));
    }
    va_end(args);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }

    // close file
    if (stream != NULL) {
        fclose(stream);
    }
}

/**
 * TODO: Proposed: similar to suggestions in create_command, let each module
 * send its own respective command, and receive the response. 
*/
char *create_command(int num_strs, ...) {
    va_list args;
    va_start(args, num_strs);

    // calculate total length of all strings
    int total_length = 0;
    for (int i = 0; i < num_strs; i++) {
        char *str = va_arg(args, char *);
        if (str != NULL)
            total_length += strlen(str) + 1;
    }
    va_end(args);

    va_start(args, num_strs);

    // create total command
    char *total_command = (char *)malloc(total_length + 1 + 2);
    check_memory(total_command);
    total_command[0] = '\0';

    // concatenate all strings
    for (int i = 0; i < num_strs; i++) {
        strcat(total_command, va_arg(args, char *));
        strcat(total_command, " ");
    }

    // add carriage return and newline
    strcat(total_command, "\r\n");

    va_end(args);
    return total_command;
}

void check_memory(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(5);
    }
}

int setup_connection(char *hostname) {
    int connfd = 0;
    int status = 0;
    struct addrinfo hints, *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    status = getaddrinfo(hostname, IMAP_PORT, &hints, &result);

    // try to connect to each address in the list
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        // check if hostname is valid
        if (status == 0) {

            // create a socket
            connfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (connfd == -1) {
                perror("socket");
                continue;
            }

            // connect to server through socket
            status = connect(connfd, rp->ai_addr, rp->ai_addrlen);
            if (status == -1) {
                perror("connect");
                fprintf(stderr, "Could not connect to %s\n", hostname);
                exit(2);
            }

            // successfully connected
            printf("Connected to %s\n\n", hostname);
            break;
        } else {
            fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(status));
        }
    }
    if (rp == NULL) {
        fprintf(stderr, "Could not connect to %s\n", hostname);
        exit(2);
    }
    freeaddrinfo(result);

    return connfd;
}

// generate a unique tag that is used to identify each command
// increments the tag each time it is called
void get_tag(char *buffer, size_t size) {
    static int tag = 0;
    if (tag < MAX_TAG) {
        snprintf(buffer, size, "%d", tag++);
    } else {
        fprintf(stderr, "Exceeded maximum number of tags\n");
        exit(5);
    }
}

/**
 * TODO: Proposed: instead of sending command in main, let each module send its
 * own respective command, and receive the response. This way, the main function
 * will only be responsible for setting up the connection, determine what command
 * is requested by tasks and close the connection.
 */ 
void send_command(char **tag, char *command, char **buffer, int connfd,
                  FILE *stream) {
    // initialise command
    get_tag(*tag, MAX_TAG_SIZE);
    char *total_command = (char *)malloc(strlen(*tag) + strlen(command) + 2);
    strcpy(total_command, *tag);
    strcat(total_command, " ");
    strcat(total_command, command);

    // send command to server
    write(connfd, total_command, strlen(total_command));
    printf("Sent: %s\n", total_command);

    // receive first response from server
    char line[MAX_LINE_SIZE];
    fgets(line, MAX_LINE_SIZE, stream);
    strcat(*buffer, line);

    // check if server is ready
    if (strncmp(*buffer, "* ", 2) != 0) {
        fprintf(stderr, "Server not ready\n");
        fprintf(stderr, "Received: %s\n", *buffer);
        exit(3);
    }

    // confirm command
    char *confirm_command = malloc(strlen(*tag) + 4);
    strcpy(confirm_command, *tag);
    strcat(confirm_command, " OK");
    while (strncmp(line, confirm_command, strlen(confirm_command)) != 0) {
        fgets(line, MAX_LINE_SIZE, stream);
        // realloc buffer if needed
        if (strlen(*buffer) + strlen(line) >= MAX_DATA_SIZE) {
            *buffer = realloc(*buffer, strlen(*buffer) * REALLOC_SIZE + 1);
            if (*buffer == NULL) {
                fprintf(stderr, "Failed to allocate memory\n");
                exit(5);
            }
        }
        strcat(*buffer, line);
        // printf("%s", line);
    }

    // free memory
    free(total_command);
    free(confirm_command);
}
