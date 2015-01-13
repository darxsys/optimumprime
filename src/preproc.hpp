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

extern void preprocGreedyStorage(std::vector<PreprocResult>& result,
    TaskData* taskData);

extern void preprocGreedyUser(std::vector<PreprocResult>& result,
    TaskData* taskData);

extern void preprocGenetic(std::vector<PreprocResult>& result,
    TaskData* taskData);

#endif // PREPROC_HPP