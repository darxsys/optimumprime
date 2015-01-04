#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.hpp"

using namespace std;

#define ASSERT(expr, fmt, ...)\
    do {\
        if (!(expr)) {\
            fprintf(stderr, "[ERROR]: " fmt "\n", ##__VA_ARGS__);\
            exit(-1);\
        }\
    } while (0)

static struct option options[] = {
    {"input", required_argument, 0, 'i'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static void help();

int main(int argc, char* argv[]) {

    char* inputPath = NULL;

    while (1) {

        char argument = getopt_long(argc, argv, "i:h", options, NULL);

        if (argument == -1) {
            break;
        }

        switch (argument) {
        case 'i':
            inputPath = optarg;
            break;
        default:
            help();
            return -1;
        }
    }

    ASSERT(inputPath != NULL, "missing option -i (input file)");

    TaskData* taskData;
    taskDataCreate(&taskData, inputPath);

    // MEĐIK

    taskDataDelete(taskData);

    return 0;
}

static void help() {
    printf(
    "usage: optimumprime -i <input file> [arguments ...]\n"
    "\n"
    "arguments:\n"
    "    -i, --input <file>\n"
    "        (required)\n"
    "        input file\n"
    "    -h, -help\n"
    "        prints out the help\n");
}