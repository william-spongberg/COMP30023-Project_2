#include "parse.h"
#include "retrieve.h"

void parse_headers(char *str_message_num, char **tag, char **buffer, int connfd,
                   FILE *stream) {
    // retrieve headers
    retrieve_headers(str_message_num, tag, buffer, connfd, stream);

    // get headers from first occurence of header title
    char *from = get_message(strstr(*buffer, "From: "));
    char *to = get_message(strstr(*buffer, "To: "));
    char *date = get_message(strstr(*buffer, "Date: "));
    char *subject = get_message(strstr(*buffer, "Subject: "));

    // print headers
    printf("From: %s\n", from);
    printf("To: %s\n", to);
    printf("Date: %s\n", date);
    printf("Subject: %s\n", subject);
    printf("\n");

    // free memory
    free(from);
    free(to);
    free(date);
    free(subject);
}

char *get_message(char *header) {
    if (header == NULL) {
        exit(3);
    }
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