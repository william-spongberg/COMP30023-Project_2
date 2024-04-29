#include "parse.h"
#include "read.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define _POSIX_C_SOURCE 200112L

#define IMAP_PORT "143"
#define LOGIN " LOGIN "
#define SELECT " SELECT "
#define MAX_TAG 10000
#define MAX_TAG_SIZE 4
#define MAX_DATASIZE 4096
#define MAX_LINESIZE 1024
#define REALLOC_SIZE 2

void get_tag(char *buffer, size_t size);
void send_command(char **tag, char *command, char *line, char **buffer,
                  int connfd, FILE *stream);

int main(int argc, char *argv[]) {
    char *username = NULL;
    char *password = NULL;
    char *folder = NULL;
    int message_num = 0;
    char *command = NULL;
    char *hostname = NULL;

    read_command_line(argc, argv, &username, &password, &folder, &message_num,
                      &command, &hostname);

    int connfd = 0;
    struct addrinfo hints, *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    int status = getaddrinfo(hostname, IMAP_PORT, &hints, &result);

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

    // initialise stream
    FILE *stream = fdopen(connfd, "r+");

    // initialise buffers
    char *buffer = malloc(MAX_DATASIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(5);
    }
    memset(buffer, 0, MAX_DATASIZE);

    char line[MAX_LINESIZE];
    memset(line, 0, sizeof(line));

    // initialise tag
    char *tag = malloc(MAX_TAG_SIZE);
    if (tag == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(5);
    }
    get_tag(tag, MAX_TAG_SIZE);

    // login command
    char *login =
        malloc(strlen(LOGIN) + strlen(username) + strlen(password) + 4);
    strcpy(login, LOGIN);
    strcat(login, username);
    strcat(login, " ");
    strcat(login, password);
    strcat(login, "\r\n");
    send_command(&tag, login, line, &buffer, connfd, stream);
    memset(line, 0, sizeof(line));
    memset(buffer, 0, MAX_DATASIZE);

    // select command
    char *select = malloc(strlen(SELECT) + strlen(folder) + 4);
    strcpy(select, SELECT);
    strcat(select, folder);
    strcat(select, "\r\n");
    send_command(&tag, select, line, &buffer, connfd, stream);
    memset(line, 0, sizeof(line));
    memset(buffer, 0, MAX_DATASIZE);

    // commands
    if (strcmp(command, "retrieve")) {
        // retrieve command
        char *retrieve = " FETCH 1 BODY.PEEK[]\r\n";
        send_command(&tag, retrieve, line, &buffer, connfd, stream);
        memset(line, 0, sizeof(line));
        memset(buffer, 0, MAX_DATASIZE);
        // free(retrieve);
    } else if (strcmp(command, "parse")) {
        // parse command
        char *fetch =
            " FETCH 1 BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n";
        send_command(&tag, fetch, line, &buffer, connfd, stream);
        parse_headers(buffer);
        memset(line, 0, sizeof(line));
        memset(buffer, 0, MAX_DATASIZE);
        // free(fetch);
    }

    // free memory
    free(tag);
    free(buffer);
    free(login);
    free(select);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }

    // close file
    if (stream != NULL) {
        fclose(stream);
    }
    return 0;
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

void send_command(char **tag, char *command, char *line, char **buffer,
                  int connfd, FILE *stream) {
    // initialise command
    get_tag(*tag, MAX_TAG_SIZE);
    char *total_command = (char *)malloc(strlen(*tag) + strlen(command) + 1);
    strcpy(total_command, *tag);
    strcat(total_command, command);

    // send command to server
    write(connfd, total_command, strlen(total_command));
    printf("Sent: %s\n", total_command);

    // receive first response from server
    fgets(line, MAX_DATASIZE, stream);
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
        fgets(line, MAX_LINESIZE, stream);
        // realloc buffer if needed
        if (strlen(*buffer) + strlen(line) >= MAX_DATASIZE) {
            *buffer = realloc(*buffer, strlen(*buffer)*REALLOC_SIZE + 1);
            if (*buffer == NULL) {
                fprintf(stderr, "Failed to allocate memory\n");
                exit(5);
            }
        }
        strcat(*buffer, line);
        // printf("%s", line);
    }
    printf("Received: %s\n", *buffer);
    printf("\n");
    free(total_command);
    free(confirm_command);
}
