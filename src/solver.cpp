#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <limits.h>
#include <iostream>
#include <vector>
#include <queue>
#include <set>

using namespace std;

#include "solver.hpp"

// reverse sort
bool cmpByDemand(const User* lhs, const User* rhs) {
    return lhs->demand > rhs->demand;
}

bool antByScore(const tuple<vector<int>, int, Solution>& la,
    const tuple<vector<int>, int, Solution>& ra) {

    return get<2>(la).cost < get<2>(ra).cost;
}

Solution solveGroupsTrivial(TaskData* data, PreprocResult* instance) {

    // creation of groups
    vector<vector<User*> > storageGroups(instance->openStorages.size());

    const vector<int>& groups = instance->representation;

    for (int i = 0; i < (int) groups.size(); ++i) {
        storageGroups[groups[i]].emplace_back(&data->users[i]);
    }

    Solution sol;
    vector<int> path;
    int costOpenStorages = 0;
    User* next;
    User* prev;
    Storage* currentStorage;

    for (int i = 0; i < (int) instance->openStorages.size(); ++i) {
        costOpenStorages += instance->openStorages[i]->cost;
    }

    // sort users by capacity, in each group
    for (int i = 0; i < (int) instance->openStorages.size(); ++i) {

        int pathCost = 0;
        int capacity = data->vehicleCapacity;
        prev = NULL;
        currentStorage = instance->openStorages[i];
        
        sort(storageGroups[i].begin(), storageGroups[i].end(), cmpByDemand);

        // for each storage, find hamiltonian paths and save them
        // strategy: take nodes in the order of appearance

        int storageCapacity = currentStorage->capacity;

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

    }

    sol.cost += costOpenStorages;
    return sol;
    // printSolution(sol, outputFile);
}

Solution solveGroupsGreedyOne(TaskData* data, PreprocResult* instance) {

    // creation of groups
    vector<vector<User*> > storageGroups(instance->openStorages.size());

    const vector<int>& groups = instance->representation;

    for (int i = 0; i < (int) groups.size(); ++i) {
        storageGroups[groups[i]].emplace_back(&data->users[i]);
    }

    Solution sol;
    vector<int> path;
    int costOpenStorages = 0;
    // User* next;
    // User* prev;
    // Storage* currentStorage;

    for (int i = 0; i < (int) instance->openStorages.size(); ++i) {

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

Solution solveGroupsAntColony(TaskData* data, PreprocResult* instance) {

    float fi = 0.3;
    float alpha = 1;
    float beta = 2;
    int numSteps = 1000;
    float q0 = 0.1;

    vector<Storage*>& openStorages = instance->openStorages;
    vector<int>& representation = instance->representation;

    vector<vector<User*> > groups(openStorages.size());

    for (int i = 0; i < (int) representation.size(); ++i) {
        groups[representation[i]].push_back(&data->users[i]);
    }

    // etas[i][j] = 1.0 / distances[i][j];
    vector<vector<vector<int> > > distances(openStorages.size());
    vector<vector<vector<float> > > pheromons(openStorages.size());

    // init
    for (int i = 0; i < (int) openStorages.size(); ++i) {

        int groupLen = groups[i].size() + 1;

        if (groupLen == 1) continue;

        float avgDistance = 0;
        int edges = (groupLen * (groupLen - 1)) / 2;

        distances[i].resize(groupLen);
        pheromons[i].resize(groupLen);

        for (int j = 0; j < groupLen; ++j) {

            distances[i][j].resize(groupLen, 0);

            if (j == 0) continue;

            float distance = euclideanDistance(*openStorages[i], *groups[i][j-1]);
            avgDistance += distance;

            distances[i][0][j] = distance;
            distances[i][j][0] = distances[i][0][j];
        }

        for (int j = 1; j < groupLen; ++j) {
            for (int k = j + 1; k < groupLen; ++k) {

                float distance = euclideanDistance(*groups[i][j-1], *groups[i][k-1]);
                avgDistance += distance;

                distances[i][j][k] = distance;
                distances[i][k][j] = distances[i][j][k];
            }
        }

        avgDistance /= (float) edges;

        for (int j = 0; j < groupLen; ++j) {
            pheromons[i][j].resize(groupLen, 1.0 / avgDistance);
        }
    }

    Solution sol;

    // ant colony opt
    for (int i = 0; i < (int) openStorages.size(); ++i) {

        Storage* storage = openStorages[i];

        int step = 0;
        int groupLen = groups[i].size();

        int antsLen = groupLen;

        if (groupLen == 0) {
            sol.cost += storage->cost;
            continue;
        }

        Solution best;
        best.cost = 1000000000;

        while (step < numSteps) {

            vector<tuple<vector<int>, int, Solution> > ants(antsLen);

            // ants do their stuff
            for (int j = 0; j < antsLen; ++j) {

                int start = rand() % groupLen + 1;

                get<0>(ants[j]).push_back(0);
                get<0>(ants[j]).push_back(start);

                get<1>(ants[j]) = data->vehicleCapacity - groups[i][start - 1]->demand;

                get<2>(ants[j]).cost += data->vehicleCost +
                    distances[i][0][start];

                set<int> s;
                for (int k = 0; k < groupLen; ++k) {
                    s.insert(k + 1);
                }
                s.erase(start);

                while (s.size() != 0) {
                    float max = 0;
                    int next = -1;

                    int curr = get<0>(ants[j]).back();

                    float q = (float) rand() / (float) RAND_MAX;
                    if (q < q0) {
                        set<int>::iterator it = s.begin();
                        advance(it, rand() % s.size());

                        if (get<1>(ants[j]) >= groups[i][*it - 1]->demand) {
                            next = *it;
                        }
                    }

                    if (next == -1) {
                        for (set<int>::iterator it = s.begin(); it != s.end(); ++it) {

                            float p = pow(pheromons[i][curr][*it], alpha) *
                                pow(1.0 / distances[i][curr][*it], beta);

                            if (max <= p && get<1>(ants[j]) >= groups[i][*it - 1]->demand) {
                                max = p;
                                next = *it;
                            }
                        }
                    }

                    if (next != -1) {
                        get<0>(ants[j]).push_back(next);
                        get<1>(ants[j]) -= groups[i][next - 1]->demand;
                        get<2>(ants[j]).cost += distances[i][curr][next];

                        s.erase(next);
                    } else {
                        get<0>(ants[j]).push_back(0);
                        get<2>(ants[j]).cycles.push_back(get<0>(ants[j]));
                        ++get<2>(ants[j]).numPaths;

                        get<0>(ants[j]).clear();
                        get<0>(ants[j]).push_back(0);

                        get<1>(ants[j]) = data->vehicleCapacity;
                        get<2>(ants[j]).cost += distances[i][curr][0] +
                            data->vehicleCost;
                    }
                }

                int curr = get<0>(ants[j]).back();

                get<0>(ants[j]).push_back(0);
                get<2>(ants[j]).cycles.push_back(get<0>(ants[j]));
                ++get<2>(ants[j]).numPaths;

                get<0>(ants[j]).clear();

                get<2>(ants[j]).cost += distances[i][curr][0];
            }

            sort(ants.begin(), ants.end(), antByScore);

            // pheromone evaporation
            for (int j = 0; j < (int) pheromons[i].size(); ++j) {
                for (int k = 0; k < (int) pheromons[i].size(); ++k) {
                    pheromons[i][j][k] *= (1 - fi);
                }
            }

            // pheromone reinforcement
            Solution* tempBest = &(get<2>(ants.front()));
            float delta = 1.0 / tempBest->cost;

            for (int j = 0; j < (int) tempBest->cycles.size(); ++j) {
                for (int k = 0; k < (int) tempBest->cycles[j].size() - 1; ++k) {
                    pheromons[i][tempBest->cycles[j][k]][tempBest->cycles[j][k + 1]] += delta;
                }
            }

            if (best.cost > tempBest->cost) {
                best = *tempBest;
            }

            ++step;
        }

        // merge cycles and costs
        for (int j = 0; j < (int) best.cycles.size(); ++j) {

            vector<int> cycle;
            cycle.push_back(storage->id);

            for (int k = 1; k < (int) best.cycles[j].size() - 1; ++k) {
                cycle.push_back(groups[i][best.cycles[j][k] - 1]->id);
            }

            sol.cycles.push_back(cycle);
            ++sol.numPaths;
        }

        sol.cost += best.cost + storage->cost;
    }

    return sol;
}