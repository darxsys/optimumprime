#ifndef GENETIC_HPP
#define GENETIC_HPP

#include "chromosom.hpp"
#include "utils.hpp"

extern Chromosom geneticGroupClients(TaskData *taskData, int populationLen, int iteration,
    std::vector<Storage*>& storages);

extern Chromosom localImprovement(Chromosom& chromosom, TaskData *taskData,
	std::vector<Storage*>& storages);

#endif // GENETIC_HPP
