#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr, fmt, ...)\
    do {\
        if (!(expr)) {\
            fprintf(stderr, "[ERROR]: " fmt "\n", ##__VA_ARGS__);\
            exit(-1);\
        }\
    } while (0)

static struct option options[] = {
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static void help();

int main(int argc, char* argv[]) {

    while (1) {

        char argument = getopt_long(argc, argv, "h", options, NULL);

        if (argument == -1) {
            break;
        }

        switch (argument) {
        default:
            help();
            return -1;
        }
    }

    return 0;
}

static void help() {
    printf(
    "usage: optimumprime [arguments ...]\n"
    "\n"
    "arguments:\n"
    "    -h, -help\n"
    "        prints out the help\n");
}