#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "preproc.hpp"
#include "solver.hpp"
#include "postproc.hpp"
#include "utils.hpp"

static void printStorageGroup(std::vector<Storage*>& storages);

int main(int argc, char* argv[]) {

    TaskData* taskData;
    taskDataCreate(&taskData, argc, argv);

    printParams(taskData->taskParams);

    srand(time(NULL));

    int userDemand = 0;

    for (int i = 0; i < taskData->userLen; ++i) {
        userDemand += taskData->users[i].demand;
    }

    std::vector<std::vector<Storage*> > storageSubsets;
    storageSubsetsCreate(storageSubsets, taskData->storages, userDemand);

    Solution solBest;
    solBest.cost = 123456789;

    for (int i = 0; i < (int) storageSubsets.size(); ++i) {

        printStorageGroup(storageSubsets[i]);

        PreprocResult* result = NULL;

        preprocGreedyUser(&result, taskData, storageSubsets[i]);

        if (result != NULL) {

            Solution sol1 = solveGroupsGreedyOne(taskData, result);
            sol1 = postprocGreedy(&sol1, taskData);

            Solution sol2 = solveGroupsAntColony(taskData, result);
            sol2 = postprocGreedy(&sol2, taskData);

            if (sol1.cost < solBest.cost) {
                solBest = sol1;
                fprintf(stderr, "[BEST][GR][GR][GR] %d\n", solBest.cost);
            }

            if (sol2.cost < solBest.cost) {
                solBest = sol2;
                fprintf(stderr, "[BEST][GR][ACO][GR] %d\n", solBest.cost);
            }

            delete result;
            result = NULL;
        }

        preprocGenetic(&result, taskData, storageSubsets[i]);

        if (result != NULL) {

            Solution sol1 = solveGroupsGreedyOne(taskData, result);
            sol1 = postprocGreedy(&sol1, taskData);

            Solution sol2 = solveGroupsAntColony(taskData, result);
            sol2 = postprocGreedy(&sol2, taskData);

            if (sol1.cost < solBest.cost) {
                solBest = sol1;
                fprintf(stderr, "[BEST][GA][GR][GR] %d\n", solBest.cost);
            }

            if (sol2.cost < solBest.cost) {
                solBest = sol2;
                fprintf(stderr, "[BEST][GA][ACO][GR] %d\n", solBest.cost);
            }

            delete result;
            result = NULL;
        }

        preprocSimulatedAnnealing(&result, taskData, storageSubsets[i]);

        if (result != NULL) {

            Solution sol1 = solveGroupsGreedyOne(taskData, result);
            sol1 = postprocGreedy(&sol1, taskData);

            Solution sol2 = solveGroupsAntColony(taskData, result);
            sol2 = postprocGreedy(&sol2, taskData);

            if (sol1.cost < solBest.cost) {
                solBest = sol1;
                fprintf(stderr, "[BEST][SA][GR][GR] %d\n", solBest.cost);
            }

            if (sol2.cost < solBest.cost) {
                solBest = sol2;
                fprintf(stderr, "[BEST][SA][ACO][GR] %d\n", solBest.cost);
            }

            delete result;
            result = NULL;
        }
    }

    printSolution(&solBest, "solution.txt");

    taskDataDelete(taskData);

    return 0;
}

static void printStorageGroup(std::vector<Storage*>& storages) {
    fprintf(stderr, "\n[Group]");
    for (int i = 0; i < (int) storages.size(); ++i) {
        fprintf(stderr, " %d", storages[i]->id);
    }
    fprintf(stderr, "\n\n");
}
