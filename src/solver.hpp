#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "utils.hpp"
#include "preproc.hpp"

Solution solveGroupsTrivial(TaskData* data, PreprocResult* preprocResult);

Solution solveGroupsGreedyOne(TaskData* data, PreprocResult* preprocResult);

Solution solveGroupsAntColony(TaskData* data, PreprocResult* preprocResult);

#endif // SOLVER_HPP