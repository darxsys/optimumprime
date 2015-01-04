#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdlib.h>
#include <vector>

#pragma once

struct TaskData {

    int storageLen;
    std::vector<std::pair<int, int> > storageCoordinates;
    std::vector<int> storageCapacity;
    std::vector<int> storageCost;
    int userLen;
    std::vector<std::pair<int, int> > userCoordinates;
    std::vector<int> userDemand;
    int vehicleCapacity;
    int vehicleCost;

    TaskData(int storageLen_, std::vector<std::pair<int, int> > storageCoordinates_,
            std::vector<int> storageCapacity_, std::vector<int> storageCost_,
            int userLen_, std::vector<std::pair<int, int> > userCoordinates_,
            std::vector<int> userDemand_, int vehicleCapacity_,
            int vehicleCost_) :
        storageLen(storageLen_), storageCoordinates(storageCoordinates_),
        storageCapacity(storageCapacity_), storageCost(storageCost_),
        userLen(userLen_), userCoordinates(userCoordinates_),
        userDemand(userDemand_), vehicleCapacity(vehicleCapacity_),
        vehicleCost(vehicleCost_) {
    }
};

extern void taskDataCreate(TaskData** taskData, char* inputPath);

extern void taskDataDelete(TaskData* taskData);

extern int euclideanDistance(const std::pair<int, int>& a,
    const std::pair<int, int>& b);

#endif // UTILS_HPP