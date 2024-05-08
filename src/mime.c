#include "mime.h"
#include "memory.h"
#include "retrieve.h"

int get_mime(char *str_message_num, char **tag, char **buffer, int connfd,
             FILE *stream) {
    // retrieve email body
    retrieve_body(str_message_num, tag, buffer, connfd, stream);
    char *body = *buffer;

    // Setup required content type for search
    char *content_type = "Content-Type: text/plain; charset=UTF-8";
    char *content_transfer_encoding[] = {"quoted-printable", "7bit", "8bit"};

    // Boundary to separate the body parts
    char *boundary_header = "boundary=";
    char *boundary_ptr = strstr(body, boundary_header);
    // Extract the boundary value
    char *boundary_start = boundary_ptr + strlen(boundary_header) + 1;
    char *boundary_end = strchr(boundary_start, '\"');
    char *boundary_value = (char *)malloc(boundary_end - boundary_start + 1);
    strncpy(boundary_value, boundary_start, boundary_end - boundary_start);
    // Add "--" to search for the boundary in the email body
    char *boundary = (char *)malloc(strlen(boundary_value) + 5);
    check_memory(boundary);
    boundary = strcat("--", boundary_value);

    char *content_type_ptr = strstr(body, content_type);

    if (content_type_ptr == NULL) {
        fprintf(stderr,
                "Content type text/plain with UTF-8 charset not found\n");
        return -1;
    }

    while (content_type_ptr != NULL) {
        // From the occurrence of matching content type found, search for
        // matching content transfer encoding
        for (int i = 0; i < 3; i++) {
            char *content_transfer_encoding_ptr =
                strstr(content_type_ptr, content_transfer_encoding[i]);
            if (content_transfer_encoding_ptr != NULL) {
                // Found matching transfer encoding, print the body part
                // separated by "--boundary"
                char *body_end =
                    strstr(content_transfer_encoding_ptr, boundary);
                char *body_start = content_transfer_encoding_ptr;
                char *body = (char *)malloc(body_end - body_start + 1);
                strncpy(body, body_start, body_end - body_start);
                printf("%s\n", body);

                // Free the allocated memory
                free(content_type_ptr);
                free(body);
                return 0;
            }

            // If no matching content transfer encoding found, search for the
            // next content type
            content_type_ptr = strstr(content_type_ptr + 1, content_type);
        }
    }

    // Free the allocated memory
    free(content_type_ptr);

    // No required content type with the desired charset and transferencoding
    // found
    return -1;
}

// TODO: old working version of mime, fix function above with this
void parse_mime(char *buffer) {
    // match mime boundary
    char *tmp = strstr(buffer, "boundary=");
    if (tmp == NULL) {
        fprintf(stderr, "No boundary found\n");
        exit(4);
    }

    // get actual boundary
    char *start = strchr(tmp, '=');
    start++;
    char *end = strchr(tmp, '\r');

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
    //printf("Boundary: %s\n", boundary);

    // get headers using position given by boundary
    char *hdr = strstr(buffer, boundary);

    start = strstr(hdr, "Content-Transfer-Encoding: ");
    check_memory(start);
    start = strchr(start, ' ') + 1;
    check_memory(start);
    end = strchr(start, '\r');
    check_memory(end);

    char *content_transfer_encoding = (char *)malloc(end - start + 2);
    check_memory(content_transfer_encoding);
    strncpy(content_transfer_encoding, start, end - start);
    content_transfer_encoding[end - start] = '\0';
    //printf("Content-Transfer-Encoding: %s\n", encoding);

    start = strstr(hdr, "Content-Type: ");
    check_memory(start);
    start = strchr(start, ' ') + 1;
    check_memory(start);
    end = strchr(start, '\r');
    check_memory(end);
    char *content_type = (char *)malloc(end - start + 1);
    strncpy(content_type, start, end - start);
    content_type[end - start] = '\0';
    //printf("Content-Type: %s\n", encoding);

    start = strstr(hdr, "\r\n\r\n");
    check_memory(start);
    start += 4;
    end = strstr(start, boundary);
    check_memory(end);
    char *message = (char *)malloc(end - start + 2);
    check_memory(message);
    strncpy(message, start, end - start);
    message[end - start - 1] = '\0';
    //printf("[Start Message]");
    printf("%s\r\n", message);
    //printf("[End Message]\n\n");

    // free memory
    free(boundary);
    free(content_transfer_encoding);
    free(content_type);
    free(message);
}
