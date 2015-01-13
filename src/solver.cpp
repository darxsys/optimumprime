#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <limits.h>
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

#include "solver.hpp"

// reverse sort
bool cmpByDemand(const User* lhs, const User* rhs) {
    return lhs->demand > rhs->demand;
}

Solution solveGroupsTrivial(TaskData* data, PreprocResult* instance) {

    Timeval groupTimer, costOpenTimer, sortTimer, wTimer;

    // creation of groups
    vector<vector<User*> > storageGroups(instance->openStorages.size());

    const vector<int>& groups = instance->representation;
    timerStart(&groupTimer);
    for (int i = 0; i < groups.size(); ++i) {
        storageGroups[groups[i]].emplace_back(&data->users[i]);
    }
    timerPrintSecs("Groups", timerStopSecs(&groupTimer));

    Solution sol;
    vector<int> path;
    int costOpenStorages = 0;
    User* next;
    User* prev;
    Storage* currentStorage;

    timerStart(&costOpenTimer);
    for (int i = 0; i < (int) instance->openStorages.size(); ++i) {
        costOpenStorages += instance->openStorages[i]->cost;
    }
    timerPrintSecs("Cost open", timerStopSecs(&costOpenTimer));

    double sortTime = 0;
    double whileTime = 0;

    // sort users by capacity, in each group
    for (int i = 0; i < (int) instance->openStorages.size(); ++i) {

        int pathCost = 0;
        int capacity = data->vehicleCapacity;
        prev = NULL;
        currentStorage = instance->openStorages[i];
        
        timerStart(&sortTimer);
        sort(storageGroups[i].begin(), storageGroups[i].end(), cmpByDemand);
        sortTime += timerStopSecs(&sortTimer);
        // for each storage, find hamiltonian paths and save them
        // strategy: take nodes in the order of appearance

        int storageCapacity = currentStorage->capacity;

        timerStart(&wTimer);
        while (!storageGroups[i].empty()) {
            // next vehicle
            pathCost += data->vehicleCost;
            next = storageGroups[i].back();
            path.emplace_back(instance->openStorages[i]->id);

            while (capacity >= next->demand) {
                storageCapacity -= next->demand;

                if (storageCapacity < 0) {
                    // can't solve
                    Solution sol;
                    sol.cost = 2000000000;
                    return sol;
                }

                capacity -= next->demand;

                if (prev == NULL) {
                    pathCost += euclideanDistance(*currentStorage, *next);
                } else {
                    pathCost += euclideanDistance(*prev, *next);
                }

                prev = next;

                path.push_back(next->id);
                storageGroups[i].pop_back();

                if (storageGroups[i].empty()) {
                    break;
                }

                next = storageGroups[i].back();
            }

            pathCost += euclideanDistance(*currentStorage, *prev);

            sol.addCycle(path, pathCost);
            path.clear();

            prev = NULL;
            pathCost = 0;
            capacity = data->vehicleCapacity;
        }

        whileTime += timerStopSecs(&wTimer);
    }

    timerPrintSecs("Sort", sortTime);
    timerPrintSecs("While", whileTime);

    sol.cost += costOpenStorages;
    return sol;
    // printSolution(sol, outputFile);
}

Solution solveGroupsGreedyOne(TaskData* data, PreprocResult* instance) {

    // creation of groups
    vector<vector<User*> > storageGroups(instance->openStorages.size());

    const vector<int>& groups = instance->representation;
    // timerStart(&groupTimer);
    for (int i = 0; i < groups.size(); ++i) {
        storageGroups[groups[i]].emplace_back(&data->users[i]);
    }
    // timerPrintSecs("Groups", timerStopSecs(&groupTimer));

    Solution sol;
    vector<int> path;
    int costOpenStorages = 0;
    User* next;
    User* prev;
    Storage* currentStorage;

    // timerStart(&costOpenTimer);
    // timerPrintSecs("Cost open", timerStopSecs(&costOpenTimer));

    for (int i = 0; i < instance->openStorages.size(); ++i) {

        costOpenStorages += instance->openStorages[i]->cost;

        // solve hamiltonian paths
        // take the closest node that is also under the capacity of the current truck
        // if no such exists, go back to the storage and start a new path
        // if the capacity of the storage is exceeded, return rubbish

        Storage* currentStorage = instance->openStorages[i];
        Node* current = instance->openStorages[i];
        int pathCost = 0;
        User* best = NULL;
        int bestDist = INT_MAX;
        int bestIndex = -1;
        int dist = 0;

        int groupSize = storageGroups[i].size();
        vector<bool> visited(groupSize, true);
        int storageCapacity = instance->openStorages[i]->capacity;
        vector<int> path;
        path.push_back(currentStorage->id);

        int numToDo = groupSize;
        int truckCapacity = data->vehicleCapacity;

        while (numToDo > 0) {
            // find next best node
            for (int j = 0; j < groupSize; ++j) {
                if (visited[j] && truckCapacity >= storageGroups[i][j]->demand) {
                    dist = euclideanDistance(*current, *storageGroups[i][j]);

                    if (dist < bestDist) {
                        best = storageGroups[i][j];
                        bestDist = dist;
                        bestIndex = j;
                    }
                }
            }

            // if nothing is found, go back to storage
            if (best == NULL) {

                pathCost += euclideanDistance(*currentStorage, *current);
                pathCost += data->vehicleCost;
                sol.addCycle(path, pathCost);
                path.clear();
                path.push_back(currentStorage->id);

                truckCapacity = data->vehicleCapacity;
                current = currentStorage;
                pathCost = 0;

            } else {

                numToDo--;
                visited[bestIndex] = false;
                path.push_back(best->id);

                truckCapacity -= best->demand;
                storageCapacity -= best->demand;
                if (storageCapacity < 0) {
                    sol.cost = INT_MAX;
                    return sol;
                }

                pathCost += bestDist;
                current = best;
                best = NULL;
                bestDist = INT_MAX;

            }
        }

        if (path.size() > 1) {
            pathCost += euclideanDistance(*currentStorage, *current);
            pathCost += data->vehicleCost;            
            sol.addCycle(path, pathCost);
        }
    }

    sol.cost += costOpenStorages;
    return sol;
}
