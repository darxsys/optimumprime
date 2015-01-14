#ifndef PREPROC_HPP
#define PREPROC_HPP

#include "utils.hpp"

struct PreprocResult {

    std::vector<Storage*> openStorages;
    std::vector<int> representation;

    PreprocResult(std::vector<Storage*> storages, std::vector<int> representation_) :
        openStorages(storages), representation(representation_) {
    }
};

extern void preprocGreedyStorage(PreprocResult** result, TaskData* data,
    std::vector<Storage*>& openStorages);

extern void preprocGreedyUser(PreprocResult** result, TaskData* data,
    std::vector<Storage*>& openStorages);

extern void preprocGenetic(PreprocResult** result, TaskData* data,
    std::vector<Storage*>& openStorages);

extern void preprocSimulatedAnnealing(PreprocResult** result, TaskData* data,
    std::vector<Storage*>& openStorages);

extern void storageSubsetsCreate(std::vector<std::vector<Storage*> >& result,
    std::vector<Storage>& storages, int minCapacity);

#endif // PREPROC_HPP