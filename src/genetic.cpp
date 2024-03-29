#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <limits.h>
#include <iostream>

#include "genetic.hpp"

extern Chromosom localImprovement(Chromosom& chromosom, TaskData* taskData,
    std::vector<Storage*>& storages);

extern Chromosom localImprovementExtra(Chromosom& chromosom, TaskData* taskData,
    std::vector<Storage*>& storages);

static Chromosom chooseParent(std::vector<Chromosom>& population) {

    double maxCost = population[population.size() - 1].cost;

    double sumFitness = 0;
    for (int i = population.size() - 1; i >= 0; i--) {
        sumFitness += (maxCost - population[i].cost);
    }

    double randNumber = (double) rand() / RAND_MAX * sumFitness;

    double number = 0;

    int max = population.size();

    for (int i = 0; i < max; i++) {

        number += (maxCost - population[i].cost);

        if (number > randNumber) {
            return population[i];
        }            
    }

    return population[population.size() - 1];
}

static int closestStorage(User& user, std::vector<Storage*>& storages) {

    double minCost = INT_MAX;
    double cost;

    int index = 0;

    for (int i = 0; i < (int) storages.size(); i++) {
        Storage* storage = storages[i];
        cost = (storage->x - user.x) * (storage->x - user.x) / 10.0 +
            (storage->y - user.y) * (storage->y - user.y) / 10.0;

        if (cost < minCost) {
            index = i;
            minCost = cost;
        }
    }

    return index;
}


extern Chromosom geneticGroupClients(TaskData *taskData, int populationLen, int iteration,
    std::vector<Storage*>& storages) {

    //initialization

    std::vector<Chromosom> population;
    std::vector<Chromosom> newPopulation;
    std::vector<Chromosom> temp;
    
    population.reserve(populationLen);
    newPopulation.reserve(populationLen);

    for (int i = 0; i < populationLen; i++) {
        population.push_back(Chromosom(storages.size(), taskData->userLen));
        newPopulation.push_back(Chromosom(storages.size(), taskData->userLen));
    }

    while(iteration > 0) {
    
        //evalute
        for (int i = 0; i < populationLen; i++) {
            population[i].calculateCost(taskData->users, storages);
        }
      
        //sort
        std::sort(population.begin(), population.end(), compareChrom);

        //elitizam
        newPopulation[0] = population[0];
        newPopulation[1] = population[1];

        //selection
        for (int i = 1; i < populationLen/2; i++) {
            Chromosom parent1 = chooseParent(population);
            Chromosom parent2 = chooseParent(population);

            int randNumber = rand();
            Chromosom child1 = Chromosom(parent1, parent2, storages.size(),
                taskData->userLen, 1, randNumber);

            child1.mutation();

            Chromosom child2 = Chromosom(parent2, parent1, storages.size(),
                taskData->userLen, 1, randNumber);

            child2.mutation();

            newPopulation[2 * i] = child1;
            newPopulation[2 * i + 1] = child2;

        }

        temp = population;
        population = newPopulation;
        newPopulation = temp;

        iteration--;
    }

    //evalute
    for (int i = 0; i < populationLen; i++) {
        population[i].calculateCost(taskData->users, storages);
    }

    //sort
    std::sort(population.begin(), population.end(), compareChrom);

    Chromosom result = population[0];
    
    std::vector<Chromosom>().swap(newPopulation);
    std::vector<Chromosom>().swap(population);

    if (result.falseFlag == false)
        result = localImprovement(result, taskData, storages);

    return result;
}


extern Chromosom localImprovement(Chromosom& chromosom, TaskData *taskData,
    std::vector<Storage*>& storages) {
    
    int size = chromosom.representation.size();

    int iteration = 1000;

    int betterIndex;
    int currentIndex;
    double prevCost;
    while(iteration > 0) {
        for (int i = 0; i < size; i++) {
            currentIndex = chromosom.representation[i];
            betterIndex = closestStorage(taskData->users[i], storages);

            if (currentIndex != betterIndex) {
                
                //try to find user for swithcing storages
                for (int j = 0; j < size; j++) {
                    if (chromosom.representation[j] == betterIndex) {
                        //switch and check solution
                        chromosom.representation[j] = currentIndex;
                        chromosom.representation[i] = betterIndex;


                        prevCost = chromosom.cost;
                        chromosom.calculateCost(taskData->users, storages);

                        //we found better solution
                        if (prevCost > chromosom.cost) {
                            break;
                        } else {
                            chromosom.representation[i] = currentIndex;
                            chromosom.representation[j] = betterIndex;
                            chromosom.cost = prevCost;
                            chromosom.falseFlag = false;
                        }
                    }
                }
            }
        }

        --iteration;
    }
    
    chromosom = localImprovementExtra(chromosom, taskData, storages);

    return chromosom;   
}

extern Chromosom localImprovementExtra(Chromosom& chromosom, TaskData *taskData,
    std::vector<Storage*>& storages) {

    bool changed = false;

    int size = chromosom.representation.size();
    int storagesLen = storages.size();

    double prevCost;
    int prevIndex;

    int iteration = 1000;
    while(iteration-- > 0){

        changed = false;
        for (int i = 0; i < size; i++) {

            for (int index = 0; index < storagesLen; index++) {
                prevIndex = chromosom.representation[i];
                prevCost = chromosom.cost;

                chromosom.representation[i] = index;
                chromosom.calculateCost(taskData->users, storages);

                //we found better solution
                if (prevCost > chromosom.cost) {
                    changed = true;
                } else {
                    chromosom.representation[i] = prevIndex;
                    chromosom.cost = prevCost;
                    chromosom.falseFlag = false;
                }
            }
        }

        if (changed == false) {
            break;
        }
    }

    return chromosom;
}

