#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdlib.h>
#include <vector>
#include <math.h>

#include "timer.h"
#include "thread.h"
#include "threadpool.h"

#include "node.hpp"
#include "user.hpp"
#include "storage.hpp"
#include "solution.hpp"

#define ASSERT(expr, fmt, ...)\
    do {\
        if (!(expr)) {\
            fprintf(stderr, "[ERROR]: " fmt "\n", ##__VA_ARGS__);\
            exit(-1);\
        }\
    } while (0)

struct TaskData {

    int storageLen;
    std::vector<Storage> storages;
    int userLen;
    std::vector<User> users;
    int vehicleCapacity;
    int vehicleCost;

    TaskData(int storageLen_, std::vector<Storage> storages_, int userLen_,
            std::vector<User> users_, int vehicleCapacity_, int vehicleCost_) :
        storageLen(storageLen_), storages(storages_), userLen(userLen_),
        users(users_), vehicleCapacity(vehicleCapacity_),
        vehicleCost(vehicleCost_) {
    }
};

extern void taskDataCreate(TaskData** taskData, char* inputPath);

extern void taskDataDelete(TaskData* taskData);

extern void printSolution(const Solution* sol, char* outputFile);

template<class T1, class T2>
extern int euclideanDistance(T1& a, T2& b) {
    double distance = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    return (int) (distance * 100);
}

#endif // UTILS_HPP