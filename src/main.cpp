#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "preproc.hpp"
#include "solver.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {

    ASSERT(argc == 2, "missing input file");

    char* inputPath = argv[1];

    TaskData* taskData;
    taskDataCreate(&taskData, inputPath);

    srand(time(NULL));

    Timeval preprocTimer;
    timerStart(&preprocTimer);

    std::vector<PreprocResult> preprocResults;

    // preprocGreedyStorage(preprocResults, taskData);
    preprocGreedyUser(preprocResults, taskData);
    preprocGenetic(preprocResults, taskData);
    // preprocSimulatedAnnealing(preprocResults, taskData);

    fprintf(stderr, "[preproc found %d subsets]\n", (int) preprocResults.size());
    timerPrint("preproc", timerStop(&preprocTimer));

    Solution best;
    best.cost = 2000000000;

    Timeval solveTimer;
    timerStart(&solveTimer);

    for (int i = 0; i < (int) preprocResults.size(); ++i) {
        printf("[%d] ", i);
        for (int j = 0; j < (int) preprocResults[i].openStorages.size(); ++j) {
            printf("%d ", preprocResults[i].openStorages[j]->id);
        }
        printf("\n");

        Solution sol = solveGroupsGreedyOne(taskData, &preprocResults[i]);

        if (best.cost > sol.cost) {
            best = sol;
            printf("BEST [Greedy] %d\n", best.cost);
        } else {
            printf("Greedy: %d\n", sol.cost);
        }

        Solution sol2 = solveGroupsAntColony(taskData, &preprocResults[i]);

        if (best.cost > sol2.cost) {
            best = sol2;
            printf("BEST [AntColony] %d\n", best.cost);
        } else {
            printf("AntColony %d\n", sol2.cost);
        }
    }

    timerPrint("solve", timerStop(&solveTimer));

    printSolution(&best, "solution.txt");

    taskDataDelete(taskData);

    return 0;
}
