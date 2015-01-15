#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <algorithm>

#include "postproc.hpp"

using namespace std;

// ***************************************************************************
// PUBLIC

extern Solution postprocGreedy(Solution* solution, TaskData* data);

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void solutionCost(Solution& sol, TaskData* data);

// ***************************************************************************



// ***************************************************************************
// PUBLIC

extern Solution postprocGreedy(Solution* solution, TaskData* data) {

    fprintf(stderr, "[Greedy] start\n");

    Timeval timer;
    timerStart(&timer);

    int population = data->taskParams.grPopulationLen;

    Solution best = *solution;

    int pathLen = solution->numPaths;

    for (int i = 0; i < population * 1000; ++i) {

        Solution curr = best;

        int path1 = rand() % pathLen;
        int path2 = rand() % pathLen;

        while (path2 == path1) {
            path2 = rand() % pathLen;
        }

        int user1 = rand() % (curr.cycles[path1].size() - 1) + 1;
        int user2 = rand() % (curr.cycles[path2].size() - 1) + 1;

        int temp = curr.cycles[path1][user1];
        curr.cycles[path1][user1] = curr.cycles[path2][user2];
        curr.cycles[path2][user2] = temp;

        solutionCost(curr, data);

        if (curr.cost == -1) { continue; }

        if (curr.cost < best.cost) {
            best = curr;
        }
    }

    timerPrintSes("[Greedy] end", timerStopSes(&timer));

    return best;
}

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void solutionCost(Solution& sol, TaskData* data) {

    int cost = 0;
    set<int> storages;

    vector<int> demands(data->storageLen, 0);

    for (int i = 0; i < sol.numPaths; ++i) {

        int demand = 0;
        for (int j = 1; j < (int) sol.cycles[i].size(); ++j) {
            demand += data->users[sol.cycles[i][j]].demand;
        }

        if (demand > data->vehicleCapacity) {
            sol.cost = -1;
            return;
        }

        cost += data->vehicleCost;

        storages.insert(sol.cycles[i][0]);

        demands[sol.cycles[i][0]] += demand;

        cost += euclideanDistance(data->storages[sol.cycles[i][0]],
            data->users[sol.cycles[i][1]]);

        for (int j = 1; j < (int) sol.cycles[i].size() - 1; ++j) {
            cost += euclideanDistance(data->users[sol.cycles[i][j]],
                data->users[sol.cycles[i][j + 1]]);
        }

        cost += euclideanDistance(data->storages[sol.cycles[i][0]],
            data->users[sol.cycles[i].back()]);
    }

    for (set<int>::iterator it = storages.begin(); it != storages.end(); ++it) {
        if (demands[*it] > data->storages[*it].capacity) {
            sol.cost = -1;
            return;
        }

        cost += data->storages[*it].cost;
    }

    sol.cost = cost;
}

// ***************************************************************************
