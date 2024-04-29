#include "parse.h"

void parse_headers(char *buffer) {
    // get headers from first occurence of header title
    char *from = strstr(buffer, "From: ");
    char *to = strstr(buffer, "To: ");
    char *date = strstr(buffer, "Date: ");
    char *subject = strstr(buffer, "Subject: ");

    // print headers or empty string or error message
    printf("From: %s\n", get_message(from));
    printf("To: %s\n", get_message(to));
    printf("Date: %s\n", get_message(date));
    printf("Subject: %s\n", get_message(subject));
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