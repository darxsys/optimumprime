#include <stdio.h>
#include <stdlib.h>

#include "utils.hpp"

#define ASSERT(expr, fmt, ...)\
    do {\
        if (!(expr)) {\
            fprintf(stderr, "[ERROR]: " fmt "\n", ##__VA_ARGS__);\
            exit(-1);\
        }\
    } while (0)


int main(int argc, char* argv[]) {

    ASSERT(argc == 2, "missing input file");

    char* inputPath = argv[1];

    TaskData* taskData;
    taskDataCreate(&taskData, inputPath);

    // MEĐIK

    taskDataDelete(taskData);

    return 0;
}