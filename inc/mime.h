#ifndef MIME_H
#define MIME_H

#include <stdio.h>
#include <string.h>

// GOAL: find the first UTF-8 text/plain body part of an email

// Fetch body of email

// identify MIME header
// Version (Always 1.0 for this project)
// Content type (Always text/plain for this project) ; boundary (AKA delimiter)

// Within the body, find the parts separated by the boundary and satisfy all of
// the following condition: Content type is text/plain Charset is UTF - 8
// Content transfer encoding quoted-printable | 7bit | 8bit

int get_mime(char *buffer);
void parse_mime(char *buffer);
#endif
