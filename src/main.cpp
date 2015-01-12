#include <stdio.h>
#include <stdlib.h>

#include "utils.hpp"
#include "preproc.hpp"

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
    std::vector<PreprocResult> preprocResults;
    preprocGreedyStorage(preprocResults, taskData);
    preprocGreedyUser(preprocResults, taskData);

    for (int i = 0; i < (int) preprocResults.size(); ++i) {
        printf("[%d] ", i);
        for (int j = 0; j < (int) taskData->userLen; ++j) {
            printf("%d ", preprocResults[i].representation[j]);
        }
        printf("\n");
    }

    taskDataDelete(taskData);

    return 0;
}