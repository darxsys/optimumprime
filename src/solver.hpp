#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "utils.hpp"
#include "preproc.hpp"

extern Solution solveGroupsTrivial(TaskData* data, PreprocResult* preprocResult);

extern Solution solveGroupsGreedyOne(TaskData* data, PreprocResult* preprocResult);

extern Solution solveGroupsAntColony(TaskData* data, PreprocResult* preprocResult);

#endif // SOLVER_HPP