#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#define _POSIX_C_SOURCE 200112L

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];
    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // connecting to port 143 (IMAP)
    int status = getaddrinfo(hostname, "143", &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // create a socket
    int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // connect to server through socket
    status = connect(sockfd, result->ai_addr, result->ai_addrlen);
    if (status == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // close socket and free addrinfo
    close(sockfd);
    freeaddrinfo(result);
    return 0;
}
