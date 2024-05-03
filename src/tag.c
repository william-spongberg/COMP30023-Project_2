#include "tag.h"

// generate a unique tag that is used to identify each command
// increments the tag each time it is called
void get_num_tag(char *buffer, size_t size) {
    static int tag = 0;
    if (tag < MAX_TAG) {
        snprintf(buffer, size, "%d", tag++);
    } else {
        fprintf(stderr, "Exceeded maximum number of tags\n");
        exit(5);
    }
}

// TODO: create other tag generators