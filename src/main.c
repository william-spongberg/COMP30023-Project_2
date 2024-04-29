#include "parse.h"
#include "read.h"
#include <netdb.h>
#include <stdio.h>
#include <stdarg.h>
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
void send_command(char **tag, char *command, char **buffer,
                  int connfd, FILE *stream);
char *create_command(int num_strs, ...);
int setup_connection(char *hostname);
void check_memory(void *args);
void free_memory(int connfd, FILE *stream, int num_ptrs, ...);

int main(int argc, char *argv[]) {
    char *username = NULL;
    char *password = NULL;
    char *folder = NULL;
    char *message_num = NULL;
    int num = 0;
    char *command = NULL;
    char *hostname = NULL;

    // read command line arguments
    read_command_line(argc, argv, &username, &password, &folder, &message_num,
                      &command, &hostname);
    num = atoi(message_num);

    // setup connection
    int connfd = setup_connection(hostname);

    // initialise stream
    FILE *stream = fdopen(connfd, "r+");

    // initialise buffer
    char *buffer = (char*) calloc(MAX_DATA_SIZE, sizeof(char));
    check_memory(buffer);
    char *tag = (char*) calloc(MAX_TAG_SIZE, sizeof(char));
    check_memory(tag);
    get_tag(tag, MAX_TAG_SIZE);

    // login command
    char *login = create_command(3, LOGIN, username, password);
    send_command(&tag, login, &buffer, connfd, stream);
    memset(buffer, 0, MAX_DATA_SIZE);
    free(login);

    // select command
    char *select = create_command(2, SELECT, folder);
    send_command(&tag, select, &buffer, connfd, stream);
    memset(buffer, 0, MAX_DATA_SIZE);
    free(select);

    // commands
    if (strcmp(command, "retrieve")) {
        // retrieve command
        char *retrieve = create_command(3, FETCH, message_num, BODY);
        send_command(&tag, retrieve, &buffer, connfd, stream);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(retrieve);
    } else if (strcmp(command, "parse")) {
        // parse command
        char *headers = create_command(3, FETCH, message_num, BODY_HEADERS);
        send_command(&tag, headers, &buffer, connfd, stream);
        parse_headers(buffer);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(headers);
    } else if (strcmp(command, "mime")) {
        // mime command
        char *retrieve = create_command(3, FETCH, message_num, BODY);
        send_command(&tag, retrieve, &buffer, connfd, stream);
        //parse_mime(buffer);
        memset(buffer, 0, MAX_DATA_SIZE);
        free(retrieve);
    }

    // free memory
    free_memory(connfd, stream, 2, tag, buffer);

    return 0;
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

void send_command(char **tag, char *command, char **buffer,
                  int connfd, FILE *stream) {
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

    // print response
    printf("Received: %s\n", *buffer);
    printf("\n");

    // free memory
    free(total_command);
    free(confirm_command);
}
