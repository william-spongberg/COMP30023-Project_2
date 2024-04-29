#include "read.h"
#include <assert.h>

void read(int argc, char *argv[], const char *hostname,
          const char *password, const char *folder, int *message_num,
          char *command, char *server_name)
{
    int opt;
    while (opt = getopt(argc, argv, "h:p:f:m:c:s:"), opt != -1) {
        switch (opt) {
        case 'h':
            hostname = strdup(optarg);
            assert(hostname != NULL);
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
            //FIXME: Need to figure out what the input format is for this tag
            // Command always comes before server name, need to know how they
            // are delimited

            break;
        default:
            fprintf(stderr, "Usage: %s -h <hostname> -p <password> "
                            "-f <folder> -n <message_num> -c <command> "
                            "-s <server_name>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}
