#include "mime.h"
#include "memory.h"
#include "retrieve.h"

int get_mime(char *buffer) {

    // printf("Buffer:\n %s\n", buffer);

    // Setup required content type for search
    char *content_type = "Content-Type: text/plain;";
    char *content_transfer_encoding[] = {"quoted-printable", "7bit", "8bit"};

    // Boundary to separate the body parts
    char *boundary_header = "boundary=";
    char *boundary_ptr = strstr(buffer, boundary_header);
    // get actual boundary
    char *start = strchr(boundary_ptr, '=');
    start++;
    char *end = strchr(boundary_ptr, '\r');

    // remove quotes if exist
    if (start[0] == '"') {
        start += 1;
        end -= 2;
    }

    // add "--" to boundary, copy to new variable
    char *boundary = (char *)malloc(end-start + 3);
    check_memory(boundary);
    strcpy(boundary, "--");
    strncat(boundary, start, end - start);
    boundary_ptr = strstr(boundary_ptr + 1, boundary);
    // printf("Boundary: %s\n", boundary);

    int has_mime = 0;

    // printf("boundary_ptr:\n %s\n", boundary_ptr);
    // Continue until end of buffer
    while (boundary_ptr != NULL) {
        // Find the content type
        char *content_type_ptr = strstr(boundary_ptr, "Content-Type: ");
        if (content_type_ptr == NULL) {
            if (has_mime == 0) {
                fprintf(stderr, "No content type found\n");
            }
            exit(6);
        }
        // Check if content type is text/plain
        if (strstr(content_type_ptr, content_type) != NULL && 
            strstr(content_type_ptr, "charset=UTF-8") != NULL &&
            ((strstr(content_type_ptr, content_transfer_encoding[0]) != NULL ||
            strstr(content_type_ptr, content_transfer_encoding[1]) != NULL ||
            strstr(content_type_ptr, content_transfer_encoding[2]) != NULL))){
            // Find the start of the message
            char *start = strstr(content_type_ptr, "\r\n\r\n");
            if (start == NULL) {
                fprintf(stderr, "No message found\n");
                exit(7);
            }
            start += 4;
            // Find the end of the message
            char *end = strstr(start, boundary);
            if (end == NULL) {
                fprintf(stderr, "No end of message found\n");
                exit(8);
            }

            char *message = (char *)malloc(end - start + 2);
            check_memory(message);
            strncpy(message, start, end - start);
            message[end - start - 1] = '\0';
            has_mime = 1;
            printf("%s\n", message);
            free(message);
        }
        // Find the next boundary
        boundary_ptr = strstr(boundary_ptr + 1, boundary);
    }

    free(boundary);
    free(buffer);

    // No required content type with the desired charset and transferencoding
    // found
    return 0;
}

// TODO: old working version of mime, fix function above with this
// void parse_mime(char *buffer) {
//     // match mime boundary
//     char *tmp = strstr(buffer, "boundary=");
//     if (tmp == NULL) {
//         fprintf(stderr, "No boundary found\n");
//         exit(4);
//     }

//     // get actual boundary
//     char *start = strchr(tmp, '=');
//     start++;
//     char *end = strchr(tmp, '\r');

//     // remove quotes if exist
//     if (start[0] == '"') {
//         start += 1;
//         end -= 2;
//     }

//     // add "--" to boundary, copy to new variable
//     char *boundary = (char *)malloc(end-start + 3);
//     check_memory(boundary);
//     strcpy(boundary, "--");
//     strncat(boundary, start, end - start);
//     //printf("Boundary: %s\n", boundary);

//     // get headers using position given by boundary
//     char *hdr = strstr(buffer, boundary);

//     start = strstr(hdr, "Content-Transfer-Encoding: ");
//     check_memory(start);
//     start = strchr(start, ' ') + 1;
//     check_memory(start);
//     end = strchr(start, '\r');
//     check_memory(end);

//     char *content_transfer_encoding = (char *)malloc(end - start + 2);
//     check_memory(content_transfer_encoding);
//     strncpy(content_transfer_encoding, start, end - start);
//     content_transfer_encoding[end - start] = '\0';
//     //printf("Content-Transfer-Encoding: %s\n", encoding);

//     start = strstr(hdr, "Content-Type: ");
//     check_memory(start);
//     start = strchr(start, ' ') + 1;
//     check_memory(start);
//     end = strchr(start, '\r');
//     check_memory(end);
//     char *content_type = (char *)malloc(end - start + 1);
//     strncpy(content_type, start, end - start);
//     content_type[end - start] = '\0';
//     //printf("Content-Type: %s\n", encoding);

//     start = strstr(hdr, "\r\n\r\n");
//     check_memory(start);
//     start += 4;
//     end = strstr(start, boundary);
//     check_memory(end);
//     char *message = (char *)malloc(end - start + 2);
//     check_memory(message);
//     strncpy(message, start, end - start);
//     message[end - start - 1] = '\0';
//     //printf("[Start Message]");
//     printf("%s\r\n", message);
//     //printf("[End Message]\n\n");

//     // free memory
//     free(boundary);
//     free(content_transfer_encoding);
//     free(content_type);
//     free(message);
// }
