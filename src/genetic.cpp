#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <limits.h>
#include <iostream>

#include "genetic.hpp"

Chromosom chooseParent(std::vector<Chromosom> population) {

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

Chromosom geneticGroupClients(TaskData *taskData, int populationLen, int iteration,
        std::vector<Storage> storages) {

    //initialization

    std::vector<Chromosom> population;
    std::vector<Chromosom> newPopulation;
    std::vector<Chromosom> temp;
    
    population.reserve(populationLen);
    newPopulation.reserve(populationLen);

    for (int i = 0; i < populationLen; i++) {
        population.push_back(Chromosom(storages.size(),taskData->userLen));
        newPopulation.push_back(Chromosom(storages.size(),taskData->userLen));
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
            Chromosom child1 = Chromosom(parent1, parent2, 
                                storages.size(), taskData->userLen, 1, randNumber);

            child1.mutation();

            Chromosom child2 = Chromosom(parent2, parent1, 
                                storages.size(), taskData->userLen, 1, randNumber);

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

    return result;
}
