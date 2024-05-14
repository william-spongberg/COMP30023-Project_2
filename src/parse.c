#include "parse.h"
#include "retrieve.h"
#include "memory.h"

void parse_headers(char *str_message_num, char **tag, char **buffer, int connfd,
                   FILE *stream) {
    // retrieve headers
    retrieve_headers(str_message_num, tag, buffer, connfd, stream);

    // get headers from first occurence of header title
    char *from = get_message(strcasestr(*buffer, "From: "));
    char *to = get_message(strcasestr(*buffer, "To: "));
    char *date = get_message(strcasestr(*buffer, "Date: "));
    char *subject = get_message(strcasestr(*buffer, "Subject: "));

    // print headers
    printf("From:%s\n", from);
    printf("To:%s\n", to);
    printf("Date:%s\n", date);
    printf("Subject:%s\n", subject);

    // free memory
    if (strlen(from) > 0){
        free(from);
    }

    if (strlen(to) > 0) {
        free(to);
    }

    if (strlen(date) > 0){
        free(date);
    }

    if (strlen(subject) > 0){
        free(subject);
    }

}

char *get_message(char *header) {
    if (header == NULL) {
        return "";
        //exit(3);
    }
    // remove first word
    char *start = strchr(header, ' ') + 1;
    check_memory(start);
    // remove trailing chars after message
    char *end = strchr(header, '\n');
    check_memory(end);

    // If the length of the message is 0, return empty string
    if (end - start <= 0) {
        char *empty = (char *)malloc(1);
        check_memory(empty);
        empty[0] = '\0';
        return empty;
    }

    // copy message
    char *message = (char *)malloc(end - start + 2);
    check_memory(message);
    strcpy(message, " ");
    strncat(message, start, end - start);
    message[end - start + 1] = '\0';
    return message;
}