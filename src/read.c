#include "read.h"
#include <assert.h>

// *TODO*
// fetchmail
// -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t]
// <command> <server_name>
// Where <command> may be one of: retrieve, parse, mime, or list

void read(int argc, char *argv[], const char *username, const char *password,
          const char *folder, int *message_num, char *command,
          char *server_name) {
    int opt;
    while (opt = getopt(argc, argv, "u:p:f:n:t"), opt != -1) {
        switch (opt) {
        case 'u':
            username = strdup(optarg);
            assert(username != NULL);
            break;
        case 'p':
            password = strdup(optarg);
            assert(password != NULL);
            break;
        case 'f':
            folder = strdup(optarg);
            assert(folder != NULL);
            break;
        case 'n':
            *message_num = atoi(optarg);
            break;
        case 't':
            // only specified if using TLS (extension task)
            break;
        default:
            fprintf(stderr,
                    "Usage: -u <username> -p <password> [-f <folder>] [-n "
                    "<messageNum>] [-t]\n <command> <server_name> \n");
            exit(1);
        }
    }

    // (optind = extra args not parsed by getopt)
    // check if all required arguments are present
    if (optind + 2 != argc) {
        fprintf(stderr, "Usage: -u <username> -p <password> [-f <folder>] [-n "
                        "<messageNum>] [-t]\n <command> <server_name> \n");
        exit(1);
    }

    // get command and server name
    command = strdup(argv[optind]);
    server_name = strdup(argv[optind + 1]);

    // check if command is valid
    if (strcmp(command, "retrieve") != 0 && strcmp(command, "parse") != 0 &&
        strcmp(command, "mime") != 0 && strcmp(command, "list") != 0) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }
}
