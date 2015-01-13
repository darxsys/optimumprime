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

    std::vector<PreprocResult> preprocResults;
    preprocGreedyStorage(preprocResults, taskData);
    preprocGreedyUser(preprocResults, taskData);

    Solution best;
    best.cost = 2000000000;

    for (int i = 0; i < (int) preprocResults.size(); ++i) {
        // Solution sol = solveGroupsGreedyOne(taskData, &preprocResults[i]);
        Solution sol2 = solveGroupsAntColony(taskData, &preprocResults[i]);

        /*if (best.cost > sol.cost) {
            best = sol;
            printf("BEST1 %d\n", best.cost);
        }*/

        if (best.cost > sol2.cost) {
            best = sol2;
            printf("BEST2 %d\n", best.cost);
        } else {
            printf("%d\n", sol2.cost);
        }
    }

    printSolution(&best, "solution.txt");

    taskDataDelete(taskData);

    return 0;
}
