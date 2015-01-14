#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <set>
#include <algorithm>

#include "utils.hpp"
#include "preproc.hpp"
#include "genetic.hpp"

using namespace std;

// ***************************************************************************
// PUBLIC

extern void preprocGreedyStorage(vector<PreprocResult>& result, TaskData* taskData);

extern void preprocGreedyUser(vector<PreprocResult>& result, TaskData* taskData);

extern void preprocGenetic(vector<PreprocResult>& result, TaskData* taskData);

extern void preprocSimulatedAnnealing(vector<PreprocResult>& result, TaskData* taskData);

// ***************************************************************************

// ***************************************************************************
// PRIVATE

// ***************************************************************************

static void storageSubsetsCreate(vector<vector<int> >& result,
    vector<Storage>& storages, int minCapacity);

static void euclideanDistancesCreate(vector<vector<int> >& result,
    vector<Storage>& storages, vector<User>& users);

static int saRepCost(vector<int>& rep, vector<int>& storages,
    vector<vector<int> >& distances);

static int validRep(vector<int>& rep, vector<int>& storages, TaskData* taskData);

// ***************************************************************************
// PUBLIC

extern void preprocGreedyStorage(vector<PreprocResult>& result, TaskData* taskData) {

    int userDemand = 0;

    for (int i = 0; i < taskData->userLen; ++i) {
        userDemand += taskData->users[i].demand;
    }

    vector<vector<int> > storageSubsets;
    storageSubsetsCreate(storageSubsets, taskData->storages, userDemand);

    vector<vector<int> > eucliedanDistances;
    euclideanDistancesCreate(eucliedanDistances, taskData->storages, taskData->users);

    for (int i = 0; i < (int) storageSubsets.size(); ++i) {

        vector<int> representation(taskData->userLen, -1);

        int valid = 1;

        for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {

            int storageIdx = storageSubsets[i][j];

            vector<pair<int, int> > users;

            for (int k = 0; k < (int) taskData->userLen; ++k) {
                if (representation[k] == -1) {
                    users.emplace_back(eucliedanDistances[k][storageIdx], taskData->users[k].id);
                }
            }

            if ((int) users.size() == 0) {
                valid = 0;
                break;
            }

            sort(users.begin(), users.end());

            int demand = 0;
            int capacity = taskData->storages[storageIdx].capacity;

            for (int k = 0; k < (int) users.size(); ++k) {
                int userIdx = users[k].second;

                if (demand + taskData->users[userIdx].demand > capacity) {
                    break;
                }

                demand += taskData->users[userIdx].demand;
                representation[userIdx] = j;
            }
        }

        for (int j = 0; j < taskData->userLen; ++j) {
            if (representation[j] == -1) {
                valid = 0;
                break;
            }
        }

        if (valid) {
            vector<Storage*> openStorages;

            for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
                openStorages.push_back(&taskData->storages[storageSubsets[i][j]]);
            }

            result.push_back(PreprocResult(openStorages, representation));
        }
    }
}

extern void preprocGreedyUser(vector<PreprocResult>& result, TaskData* taskData) {

    int userDemand = 0;

    for (int i = 0; i < taskData->userLen; ++i) {
        userDemand += taskData->users[i].demand;
    }

    vector<vector<int> > storageSubsets;
    storageSubsetsCreate(storageSubsets, taskData->storages, userDemand);

    vector<vector<int> > eucliedanDistances;
    euclideanDistancesCreate(eucliedanDistances, taskData->storages, taskData->users);

    for (int i = 0; i < (int) storageSubsets.size(); ++i) {

        if (storageSubsets[i][0] != 0) continue;
        if (storageSubsets[i][1] != 1) continue;
        if (storageSubsets[i][2] != 4) continue;

        vector<int> repBest(taskData->userLen, -1);
        int costBest = 1000000000;

        for (int k = 0; k < 50; ++k) {

            vector<pair<int, int> > users;

            for (int j = 0; j < (int) taskData->users.size(); ++j) {
                users.emplace_back(rand(), j);
            }

            sort(users.begin(), users.end());

            vector<int> representation(taskData->userLen, -1);
            vector<int> storageDemands((int) storageSubsets[i].size(), 0);

            for (int j = 0; j < (int) taskData->users.size(); ++j) {

                int userIdx = users[j].second;
                vector<tuple<int, int, int> > storages;

                for (int k = 0; k < (int) storageSubsets[i].size(); ++k) {
                    int storageIdx = storageSubsets[i][k];
                    storages.emplace_back(eucliedanDistances[userIdx][storageIdx],
                        storageIdx, k);
                }

                sort(storages.begin(), storages.end());

                for (int k = 0; k < (int) storageSubsets[i].size(); ++k) {
                    int storageIdx = get<1>(storages[k]);
                    int idx = get<2>(storages[k]);

                    if (storageDemands[idx] + taskData->users[userIdx].demand >
                        taskData->storages[storageIdx].capacity) {
                        continue;
                    }

                    storageDemands[idx] += taskData->users[userIdx].demand;
                    representation[userIdx] = idx;
                    break;
                }

            }

            int valid = 1;

            for (int j = 0; j < taskData->userLen; ++j) {
                if (representation[j] == -1) {
                    valid = 0;
                    break;
                }
            }

            if (valid) {
                int costCurr = saRepCost(representation, storageSubsets[i], eucliedanDistances);

                if (costCurr < costBest) {
                    repBest = representation;
                    costBest = costCurr;
                }
            }
        }

        vector<Storage*> openStorages;

        for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
            openStorages.push_back(&taskData->storages[storageSubsets[i][j]]);
        }

        result.push_back(PreprocResult(openStorages, repBest));
    }
}

extern void preprocGenetic(vector<PreprocResult>& result, TaskData* taskData) {

    int userDemand = 0;

    for (int i = 0; i < taskData->userLen; ++i) {
        userDemand += taskData->users[i].demand;
    }

    vector<vector<int> > storageSubsets;
    storageSubsetsCreate(storageSubsets, taskData->storages, userDemand);

    for (int i = 0; i < (int) storageSubsets.size(); ++i) {

        if (storageSubsets[i][0] != 0) continue;
        if (storageSubsets[i][1] != 1) continue;
        if (storageSubsets[i][2] != 4) continue;

        vector<Storage> openStorages;

        for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
            openStorages.push_back(taskData->storages[storageSubsets[i][j]]);
        }

        Chromosom chromosom = geneticGroupClients(taskData, 50, 75000,
            openStorages);

        vector<Storage*> openStoragesPtrs;

        for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
            openStoragesPtrs.push_back(&taskData->storages[storageSubsets[i][j]]);
        }

        result.push_back(PreprocResult(openStoragesPtrs, chromosom.representation));
    }
}

extern void preprocSimulatedAnnealing(vector<PreprocResult>& result, TaskData* taskData) {

    int userDemand = 0;

    for (int i = 0; i < taskData->userLen; ++i) {
        userDemand += taskData->users[i].demand;
    }

    vector<vector<int> > storageSubsets;
    storageSubsetsCreate(storageSubsets, taskData->storages, userDemand);

    vector<vector<int> > eucliedanDistances;
    euclideanDistancesCreate(eucliedanDistances, taskData->storages, taskData->users);

    for (int i = 0; i < (int) storageSubsets.size(); ++i) {

        int openStoragesLen = storageSubsets[i].size();

        vector<int> capacities(openStoragesLen, 0);
        vector<int> init(taskData->userLen, -1);

        int valid = 1;

        // s0
        for (int j = 0; j < taskData->userLen; ++j) {
            for (int k = 0; k < openStoragesLen; ++k) {
                if (taskData->users[j].demand + capacities[k] >
                        taskData->storages[storageSubsets[i][k]].capacity) {
                    continue;
                }

                init[j] = k;
                capacities[k] += taskData->users[j].demand;
                break;
            }

            if (init[j] == -1) {
                valid = 0;
                break;
            }
        }

        if (!valid) continue;

        if (!validRep(init, storageSubsets[i], taskData)) {      
            printf("INIT not valid\n");
        }

        vector<int> repCurr(init);
        int costCurr = saRepCost(repCurr, storageSubsets[i], eucliedanDistances);

        vector<int> repBest(repCurr);
        int costBest = costCurr;

        // t0, tf, lambda
        double temperature = 550.0;
        double tempMin = 0.01;
        double lambda = 0.99;

        vector<int> capacitiesCurr(capacities);

        while (temperature > tempMin) {

            // select a valid neighbour
            vector<int> neighbour(repCurr);

            set<int> changeList;
            int changeListLen = taskData->userLen / 33;

            while ((int) changeList.size() != changeListLen) {
                changeList.insert(rand() % taskData->userLen);
            }

            vector<int> capacitiesNew(capacitiesCurr);

            set<int>::iterator it;
            for (it = changeList.begin(); it != changeList.end(); ++it) {

                while (true) {
                    int demand = taskData->users[*it].demand;

                    capacitiesNew[neighbour[*it]] -= demand;

                    neighbour[*it] = (neighbour[*it] + 1) % openStoragesLen;

                    int idx = neighbour[*it];

                    capacitiesNew[idx] += demand;

                    if (taskData->storages[storageSubsets[i][idx]].capacity >=
                            capacitiesNew[idx]) {
                        break;
                    }
                }
            }

            if (!validRep(neighbour, storageSubsets[i], taskData)) {      
                printf("NEIGHBOUR not valid\n");
            }

            int costNew = saRepCost(neighbour, storageSubsets[i], eucliedanDistances);
            int costDiff = abs(costCurr - costNew);

            if (costNew < costCurr) {

                repCurr = neighbour;
                costCurr = costNew;
                capacitiesCurr = capacitiesNew;

            } else if (((double) rand() / (double) RAND_MAX) <
                    exp(-1.0 * costDiff / temperature)) {

                repCurr = neighbour;
                costCurr = costNew;
                capacitiesCurr = capacitiesNew;
            }

            if (costCurr < costBest) {
                repBest = repCurr;
                costBest = costCurr;
            }

            temperature *= lambda;
        }

        vector<Storage*> openStorages;

        for (int j = 0; j < (int) openStoragesLen; ++j) {
            openStorages.push_back(&taskData->storages[storageSubsets[i][j]]);
        }

        result.push_back(PreprocResult(openStorages, repBest));
    }
}

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void storageSubsetsCreate(vector<vector<int> >& result,
    vector<Storage>& storages, int minCapacity) {

    int permutationLen = (int) pow(2, (int) storages.size());

    for (int i = 0; i < permutationLen; ++i) {

        int capacity = 0;
        vector<int> permutation;

        for (int j = 0; j < (int) storages.size(); ++j) {
            if ((1 << j) & i) {
                permutation.push_back(j);
                capacity += storages[j].capacity;
            }
        }

        if (capacity < minCapacity) {
            continue;
        }

        result.push_back(permutation);
    }
}

static void euclideanDistancesCreate(vector<vector<int> >& result,
    vector<Storage>& storages, vector<User>& users) {

    result.resize((int) users.size());

    for (int i = 0; i < (int) users.size(); ++i) {
        for (int j = 0; j < (int) storages.size(); ++j) {
            result[i].push_back(euclideanDistance(users[i], storages[j]));
        }
    }
}

static int saRepCost(vector<int>& rep, vector<int>& storages,
    vector<vector<int> >& distances) {

    int cost = 0;

    for (int i = 0; i < (int) rep.size(); ++i) {
        int storageIdx = storages[rep[i]];

        cost += distances[i][storageIdx];
    }

    return cost;
}

static int validRep(vector<int>& rep, vector<int>& storages, TaskData* taskData) {

    int openStoragesLen = storages.size();

    vector<int> capacities(openStoragesLen, 0);

    for (int i = 0; i < taskData->userLen; ++i) {
        capacities[rep[i]] += taskData->users[i].demand;
    }

    for (int i = 0; i < openStoragesLen; ++i) {
        if (capacities[i] > taskData->storages[storages[i]].capacity) {
            return 0;
        }
    }

    return 1;
}

// ***************************************************************************
