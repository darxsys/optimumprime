#ifndef PREPROC_HPP
#define PREPROC_HPP

struct PreprocResult {

    std::vector<int> openStorages;
    std::vector<int> representation;

    PreprocResult(std::vector<int> storages, std::vector<int> representation_) :
        openStorages(storages), representation(representation_) {
    }
};

extern void preprocGreedyStorage(std::vector<PreprocResult>& result,
    TaskData* taskData);

extern void preprocGreedyUser(std::vector<PreprocResult>& result,
    TaskData* taskData);

#endif // PREPROC_HPP