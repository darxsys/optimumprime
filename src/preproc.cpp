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

extern void preprocGreedyStorage(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages);

extern void preprocGreedyUser(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages);

extern void preprocGenetic(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages);

extern void preprocSimulatedAnnealing(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages);

extern void storageSubsetsCreate(vector<vector<Storage*> >& result,
    vector<Storage>& storages, int minCapacity);

// ***************************************************************************

// ***************************************************************************
// PRIVATE

// ***************************************************************************

static void euclideanDistancesCreate(vector<vector<int> >& result,
    vector<Storage*>& storages, vector<User>& users);

static int repCost(vector<int>& rep, vector<vector<int> >& distances);

static int repValid(vector<int>& rep, vector<Storage*>& storages, TaskData* data);

// ***************************************************************************
// PUBLIC

extern void preprocGreedyStorage(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages) {

    fprintf(stderr, "[GreedyStorage] start\n");

    Timeval timer;
    timerStart(&timer);

    vector<vector<int> > eucliedanDistances;
    euclideanDistancesCreate(eucliedanDistances, openStorages, data->users);

    vector<int> representation(data->userLen, -1);

    for (int i = 0; i < (int) openStorages.size(); ++i) {

        vector<pair<int, int> > users;

        for (int j = 0; j < (int) data->userLen; ++j) {
            if (representation[j] == -1) {
                users.emplace_back(eucliedanDistances[j][i], data->users[j].id);
            }
        }

        if ((int) users.size() == 0) {
            continue;
        }

        sort(users.begin(), users.end());

        int demand = 0;
        int capacity = openStorages[i]->capacity;

        for (int j = 0; j < (int) users.size(); ++j) {
            int userIdx = users[j].second;

            if (demand + data->users[userIdx].demand > capacity) {
                break;
            }

            demand += data->users[userIdx].demand;
            representation[userIdx] = i;
        }
    }

    if (repValid(representation, openStorages, data)) {
        *result = new PreprocResult(openStorages, representation);
    } else {
        fprintf(stderr, "[GreedyStorage] invalid representaiton\n");
    }

    timerPrintSes("[GreedyStorage] end", timerStopSes(&timer));
}

extern void preprocGreedyUser(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages) {

    fprintf(stderr, "[GreedyUser] start\n");

    Timeval timer;
    timerStart(&timer);

    int population = data->taskParams.grPopulationLen;

    vector<vector<int> > eucliedanDistances;
    euclideanDistancesCreate(eucliedanDistances, openStorages, data->users);

    vector<int> repBest(data->userLen, -1);
    int costBest = 1000000000;

    for (int i = 0; i < population; ++i) {

        vector<pair<int, int> > users;

        for (int j = 0; j < (int) data->users.size(); ++j) {
            users.emplace_back(rand(), j);
        }

        sort(users.begin(), users.end());

        vector<int> rep(data->userLen, -1);
        vector<int> storageDemands((int) openStorages.size(), 0);

        for (int j = 0; j < (int) data->users.size(); ++j) {

            int userIdx = users[j].second;
            vector<pair<int, int> > storages;

            for (int k = 0; k < (int) openStorages.size(); ++k) {
                storages.emplace_back(eucliedanDistances[userIdx][k], k);
            }

            sort(storages.begin(), storages.end());

            for (int k = 0; k < (int) openStorages.size(); ++k) {
                int idx = storages[k].second;

                if (storageDemands[idx] + data->users[userIdx].demand >
                    openStorages[idx]->capacity) {
                    continue;
                }

                storageDemands[idx] += data->users[userIdx].demand;
                rep[userIdx] = idx;
                break;
            }
        }

        if (repValid(rep, openStorages, data)) {
            int costCurr = repCost(rep, eucliedanDistances);

            if (costCurr < costBest) {
                repBest = rep;
                costBest = costCurr;
            }
        }
    }
    
    if (repValid(repBest, openStorages, data)) {
        *result = new PreprocResult(openStorages, repBest);   
    } else {
        fprintf(stderr, "[GreedyUser] invalid representaiton\n");
    }

    timerPrintSes("[GreedyUser] end", timerStopSes(&timer));
}

extern void preprocGenetic(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStoragesPtrs) {

    fprintf(stderr, "[GeneticAlgorithm] start\n");

    Timeval timer;
    timerStart(&timer);

    int population = data->taskParams.gaPopulationLen;
    int iterations = data->taskParams.gaIterationsLen;

    vector<Storage> openStorages;

    for (int i = 0; i < (int) openStoragesPtrs.size(); ++i) {
        openStorages.push_back(*openStoragesPtrs[i]);
    }

    Chromosom chromosom = geneticGroupClients(data, population, iterations,
            openStorages);

    if (repValid(chromosom.representation, openStoragesPtrs, data)) {
        *result = new PreprocResult(openStoragesPtrs, chromosom.representation);
    } else {
        fprintf(stderr, "[GeneticAlgorithm] invalid representaiton\n");
    }

    timerPrintSes("[GeneticAlgorithm] end", timerStopSes(&timer));
}

extern void preprocSimulatedAnnealing(PreprocResult** result, TaskData* data,
    vector<Storage*>& openStorages) {

    fprintf(stderr, "[SimulatedAnnealing] start\n");

    Timeval timer;
    timerStart(&timer);

    double temperature = data->taskParams.saTemperature;
    double tempMin = data->taskParams.saMinTemperature;
    double lambda = data->taskParams.saLambda;
    int initSolution = data->taskParams.saInitSolution;

    vector<vector<int> > eucliedanDistances;
    euclideanDistancesCreate(eucliedanDistances, openStorages, data->users);

    int openStoragesLen = openStorages.size();

    vector<int> capacities(openStoragesLen, 0);
    vector<int> init(data->userLen, -1);

    if (initSolution == 0) {

        for (int i = 0; i < data->userLen; ++i) {
            for (int j = 0; j < openStoragesLen; ++j) {
                if (data->users[i].demand + capacities[j] >
                        openStorages[j]->capacity) {
                    continue;
                }

                init[i] = j;
                capacities[j] += data->users[i].demand;
                break;
            }
        }

    } else if (initSolution == 1) {
        PreprocResult* result = NULL;
        preprocGreedyUser(&result, data, openStorages);

        if (result!= NULL) {
            init = result->representation;

            for (int i = 0; i < data->userLen; ++i) {
                capacities[init[i]] += data->users[i].demand;
            }

            delete result;
        }

    } else {
        PreprocResult* result = NULL;
        preprocGenetic(&result, data, openStorages);

        if (result!= NULL) {
            init = result->representation;

            for (int i = 0; i < data->userLen; ++i) {
                capacities[init[i]] += data->users[i].demand;
            }

            delete result;
        }
    }

    if (!repValid(init, openStorages, data)) {
        fprintf(stderr, "[SimulatedAnnealing] invalid init representaiton\n");
        return;
    }

    vector<int> repBest(init);
    int costBest = repCost(repBest, eucliedanDistances);

    vector<int> capacitiesCurr(capacities);

    while (temperature > tempMin) {

        // select a valid neighbour
        vector<int> neighbour(repBest);

        for (int i = 0; i < data->userLen; ++i) {

        }

        set<int> changeList;
        int changeListLen = data->userLen / 25;

        while ((int) changeList.size() != changeListLen) {
            changeList.insert(rand() % data->userLen);
        }

        vector<int> capacitiesNew(capacitiesCurr);

        set<int>::iterator it;
        for (it = changeList.begin(); it != changeList.end(); ++it) {

            while (true) {
                int demand = data->users[*it].demand;

                capacitiesNew[neighbour[*it]] -= demand;

                neighbour[*it] = (neighbour[*it] + 1) % openStoragesLen;

                int idx = neighbour[*it];

                capacitiesNew[idx] += demand;

                if (openStorages[idx]->capacity >= capacitiesNew[idx]) {
                    break;
                }
            }
        }

        int costNew = repCost(neighbour, eucliedanDistances);
        int costDiff = abs(costBest - costNew);

        if (costNew < costBest) {

            repBest = neighbour;
            costBest = costNew;
            capacitiesCurr = capacitiesNew;

        } else if (((double) rand() / (double) RAND_MAX) <
                exp(-1.0 * costDiff / temperature)) {

            repBest = neighbour;
            costBest = costNew;
            capacitiesCurr = capacitiesNew;
        }

        temperature *= lambda;
    }

    if (repValid(repBest, openStorages, data)) {
        *result = new PreprocResult(openStorages, repBest);
    } else {
        fprintf(stderr, "[SimulatedAnnealing] invalid representaiton\n");
    }

    timerPrintSes("[SimulatedAnnealing] end", timerStopSes(&timer));
}

extern void storageSubsetsCreate(vector<vector<Storage*> >& result,
    vector<Storage>& storages, int minCapacity) {

    int permutationLen = (int) pow(2, (int) storages.size());

    for (int i = 0; i < permutationLen; ++i) {

        int capacity = 0;
        vector<Storage*> permutation;

        for (int j = 0; j < (int) storages.size(); ++j) {
            if ((1 << j) & i) {
                permutation.push_back(&storages[j]);
                capacity += storages[j].capacity;
            }
        }

        if (capacity < minCapacity) {
            continue;
        }

        result.push_back(permutation);
    }        
}

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void euclideanDistancesCreate(vector<vector<int> >& result,
    vector<Storage*>& storages, vector<User>& users) {

    result.resize((int) users.size());

    for (int i = 0; i < (int) users.size(); ++i) {
        for (int j = 0; j < (int) storages.size(); ++j) {
            result[i].push_back(euclideanDistance(users[i], *storages[j]));
        }
    }
}

static int repCost(vector<int>& rep, vector<vector<int> >& distances) {

    int cost = 0;

    for (int i = 0; i < (int) rep.size(); ++i) {
        cost += distances[i][rep[i]];
    }

    return cost;
}

static int repValid(vector<int>& rep, vector<Storage*>& storages, TaskData* data) {

    int openStoragesLen = storages.size();

    vector<int> capacities(openStoragesLen, 0);

    for (int i = 0; i < data->userLen; ++i) {
        if (rep[i] == -1) {
            return 0;
        }

        capacities[rep[i]] += data->users[i].demand;
    }

    for (int i = 0; i < openStoragesLen; ++i) {
        if (capacities[i] > storages[i]->capacity) {
            return 0;
        }
    }

    return 1;
}

// ***************************************************************************
