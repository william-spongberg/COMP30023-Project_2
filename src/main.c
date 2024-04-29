#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define _POSIX_C_SOURCE 200112L

#define IMAP_PORT "143"
#define MAX_TAG 10000
#define MAX_TAG_SIZE 4
#define MAX_DATASIZE 4096
#define MAX_LINESIZE 1024

void get_tag(char *buffer, size_t size);
void send_command(char *tag, char *command, char *line, char **buffer,
                  int connfd, FILE *stream);
char *get_message(char *header);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // *TODO*
    // fetchmail
    // -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t]
    // <command> <server_name>
    // Where <command> may be one of: retrieve, parse, mime, or list

    int connfd = 0;
    const char *hostname = argv[1];
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
                continue;
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
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    // initialise stream
    FILE *stream = fdopen(connfd, "r+");

    // initialise buffers
    char *buffer = malloc(MAX_DATASIZE);
    char line[MAX_LINESIZE];
    memset(line, 0, sizeof(line));
    memset(buffer, 0, sizeof(buffer));

    // initialise tag
    char *tag = malloc(MAX_TAG_SIZE);
    get_tag(tag, MAX_TAG_SIZE);

    // login command
    char *login = " LOGIN test@comp30023 pass\r\n";
    send_command(tag, login, line, &buffer, connfd, stream);
    memset(buffer, 0, sizeof(buffer));

    // select command
    char *select = " SELECT Test\r\n";
    send_command(tag, select, line, &buffer, connfd, stream);
    memset(buffer, 0, sizeof(buffer));

    // retrieve command
    char *retrieve = " FETCH 1 BODY.PEEK[]\r\n";
    send_command(tag, retrieve, line, &buffer, connfd, stream);
    memset(buffer, 0, sizeof(buffer));

    // parse command
    char *fetch =
        " FETCH 1 BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n";
    send_command(tag, fetch, line, &buffer, connfd, stream);

    // get headers from first occurence of header title
    char *from = strstr(buffer, "From: ");
    char *to = strstr(buffer, "To: ");
    char *date = strstr(buffer, "Date: ");
    char *subject = strstr(buffer, "Subject: ");

    // print headers or empty string or error message
    printf("From: %s\n", get_message(from) ? get_message(from) : "");
    printf("To: %s\n", get_message(to) ? get_message(to) : "");
    printf("Date: %s\n", get_message(date) ? get_message(date) : "");
    printf("Subject: %s\n", get_message(subject) ? get_message(subject) : "<No subject>");

    memset(buffer, 0, sizeof(buffer));

    // free memory
    free(tag);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }
    return 0;
}

char *get_message(char *header) {
    // remove first word
    char *start = strchr(header, ' ') + 1;
    // remove trailing chars after message
    char *end = strchr(header, '\r');
    // copy message
    char *message = (char *)malloc(end - start + 1);
    strncpy(message, start, end - start);
    message[end - start] = '\0';
    return message;
}

// generate a unique tag that is used to identify each command
// increments the tag each time it is called
void get_tag(char *buffer, size_t size) {
    static int tag = 0;
    if (tag < MAX_TAG) {
        snprintf(buffer, size, "%d", tag++);
    } else {
        fprintf(stderr, "Exceeded maximum number of tags\n");
        exit(EXIT_FAILURE);
    }
}

void send_command(char *tag, char *command, char *line, char **buffer,
                  int connfd, FILE *stream) {
    // initialise command
    get_tag(tag, MAX_TAG_SIZE);
    char *total_command = (char *)malloc(strlen(tag) + strlen(command) + 1);
    strcpy(total_command, tag);
    strcat(total_command, command);

    // send command to server
    write(connfd, total_command, strlen(total_command));
    printf("Sent: %s\n", total_command);

    // confirm command
    char *confirm_command = strcat(tag, " OK");
    while (strncmp(line, confirm_command, strlen(confirm_command)) != 0) {
        fgets(line, MAX_DATASIZE, stream);
        strcat(*buffer, line);
        //printf("%s", line);
    }
    printf("Received: %s\n", *buffer);
    printf("\n");
    free(total_command);
}
