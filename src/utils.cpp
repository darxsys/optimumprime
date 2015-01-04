#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "utils.hpp"

#define BUFFER_SIZE 1024

#define ASSERT(expr, fmt, ...)\
    do {\
        if (!(expr)) {\
            fprintf(stderr, "[ERROR]: " fmt "\n", ##__VA_ARGS__);\
            exit(-1);\
        }\
    } while (0)

using namespace std;

// ***************************************************************************
// PUBLIC

extern void taskDataCreate(TaskData** taskData, char* inputPath);

extern void taskDataDelete(TaskData* taskData);

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void parseFile(int* storageLen, vector<pair<int, int> >& storageCoordinates,
    vector<int>& storageCapacity, vector<int>& storageCost, int* userLen,
    vector<pair<int, int> >& userCoordinates, vector<int>& userDemand,
    int* vehicleCapacity, int* vehicleCost, char* inputPath);

// ***************************************************************************



// ***************************************************************************
// PUBLIC

extern void taskDataCreate(TaskData** taskData, char* inputPath) {

    int storageLen = 0;
    vector<pair<int, int> > storageCoordinates;
    vector<int> storageCapacity;
    vector<int> storageCost;

    int userLen = 0;
    vector<pair<int, int> > userCoordinates;
    vector<int> userDemand;

    int vehicleCapacity = 0;
    int vehicleCost = -1;

    parseFile(&storageLen, storageCoordinates, storageCapacity,
        storageCost, &userLen, userCoordinates, userDemand,
        &vehicleCapacity, &vehicleCost, inputPath);

    ASSERT(userLen > 0, "invalid number of users");
    ASSERT(storageLen > 0, "invalid number of storages");
    ASSERT((int) storageCoordinates.size() == storageLen, "invalid number of storage coordinates");
    ASSERT((int) userCoordinates.size() == userLen, "invalid number of user coordinates");
    ASSERT(vehicleCapacity > 0, "invalid vehicle capacity");
    ASSERT(vehicleCost > -1, "invalid vehicle cost");

    vector<Storage> storages;
    storages.reserve(storageLen);

    vector<User> users;
    users.reserve(userLen);

    for (int i = 0; i < storageLen; ++i) {
        Storage storage(storageCoordinates[i].first, storageCoordinates[i].second,
            storageCapacity[i], storageCost[i]); 

        storages.push_back(storage);
    }

    for (int i = 0; i < userLen; ++i) {
        User user(userCoordinates[i].first, userCoordinates[i].second,
            userDemand[i]); 

        users.push_back(user);
    }

    *taskData = new TaskData(storageLen, storages, userLen, users,
        vehicleCapacity, vehicleCost);
}

extern void taskDataDelete(TaskData* taskData) {
    delete taskData;
}

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void parseFile(int* storageLen, vector<pair<int, int> >& storageCoordinates,
    vector<int>& storageCapacity, vector<int>& storageCost, int* userLen,
    vector<pair<int, int> >& userCoordinates, vector<int>& userDemand,
    int* vehicleCapacity, int* vehicleCost, char* inputPath) {

    FILE* inputFile = fopen(inputPath, "r");

    char* buffer = new char[BUFFER_SIZE];
    char* garbage = new char[BUFFER_SIZE];

    fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", userLen, garbage);

    fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", storageLen, garbage);

    fgets(buffer, BUFFER_SIZE, inputFile);

    userCoordinates.reserve(*userLen);
    userDemand.reserve(*userLen);

    storageCoordinates.reserve(*storageLen);
    storageCapacity.reserve(*storageLen);
    storageCost.reserve(*storageLen);

    for (int i = 0; i < *storageLen; ++i) {
        int x, y;

        fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d\t%d%s", &x, &y, garbage);

        storageCoordinates.emplace_back(x, y);
    }

    fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *userLen; ++i) {
        int x, y;

        fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d\t%d%s", &x, &y, garbage);

        userCoordinates.emplace_back(x, y);
    }

    fgets(buffer, BUFFER_SIZE, inputFile);

    fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", vehicleCapacity, garbage);

    fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *storageLen; ++i) {
        int capacity;

        fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &capacity, garbage);

        storageCapacity.emplace_back(capacity);
    }

    fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *userLen; ++i) {
        int demand;

        fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &demand, garbage);

        userDemand.emplace_back(demand);
    }

    fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *storageLen; ++i) {
        int cost;

        fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &cost, garbage);

        storageCost.emplace_back(cost);
    }

    fgets(buffer, BUFFER_SIZE, inputFile);

    fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", vehicleCost, garbage);

    delete[] garbage;
    delete[] buffer;

    fclose(inputFile);
}

// ***************************************************************************