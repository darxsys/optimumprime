#ifndef GENETIC_HPP
#define GENETIC_HPP

#include "chromosom.hpp"
#include "utils.hpp"

Chromosom geneticGroupClients(TaskData *taskData, int populationLen, int iteration,
        std::vector<Storage> storages);

#endif // GENETIC_HPP