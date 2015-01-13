#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
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

// ***************************************************************************

// ***************************************************************************
// PRIVATE

// ***************************************************************************

static void storageSubsetsCreate(vector<vector<int> >& result,
    vector<Storage>& storages, int minCapacity);

static void euclideanDistancesCreate(vector<vector<int> >& result,
    vector<Storage>& storages, vector<User>& users);

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

        vector<int> representation(taskData->userLen, -1);
        vector<int> storageDemands((int) storageSubsets[i].size(), 0);

        for (int j = 0; j < (int) taskData->users.size(); ++j) {

            vector<tuple<int, int, int> > storages;

            for (int k = 0; k < (int) storageSubsets[i].size(); ++k) {
                int storageIdx = storageSubsets[i][k];
                storages.emplace_back(eucliedanDistances[j][storageIdx], storageIdx, k);
            }

            sort(storages.begin(), storages.end());

            for (int k = 0; k < (int) storageSubsets[i].size(); ++k) {
                int storageIdx = get<1>(storages[k]);
                int idx = get<2>(storages[k]);

                if (storageDemands[idx] + taskData->users[j].demand >
                    taskData->storages[storageIdx].capacity) {
                    continue;
                }

                storageDemands[idx] += taskData->users[j].demand;
                representation[j] = idx;
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
            vector<Storage*> openStorages;

            for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
                openStorages.push_back(&taskData->storages[storageSubsets[i][j]]);
            }

            result.push_back(PreprocResult(openStorages, representation));
        }
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

        vector<Storage> openStorages;

        for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
            openStorages.push_back(taskData->storages[storageSubsets[i][j]]);
        }

        Chromosom chromosom = geneticGroupClients(taskData, 70, 15000,
            openStorages);

        vector<Storage*> openStoragesPtrs;

        for (int j = 0; j < (int) storageSubsets[i].size(); ++j) {
            openStoragesPtrs.push_back(&taskData->storages[storageSubsets[i][j]]);
        }

        result.push_back(PreprocResult(openStoragesPtrs, chromosom.representation));
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

// ***************************************************************************
