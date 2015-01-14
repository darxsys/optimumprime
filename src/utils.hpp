#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdlib.h>
#include <vector>
#include <math.h>

#include "timer.h"

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

struct TaskParams {

    // [Preproc] Genetic algorithm
    int gaPopulationLen;
    int gaIterationsLen;

    // [Preproc] Greedy
    int grPopulationLen;

    // [Preproc] Simulated annealing
    double saTemperature;
    double saMinTemperature;
    double saLambda;
    int saInitSolution; // 0 - random, 1 - greedy, 2 - genetic

    // [Solver] Ant colony optimization
    double acoFi;
    double acoAlpha;
    double acoBeta;
    double acoQ0;
    int acoPopulationLen; // default = user length
    int acoIterationsLen;

    TaskParams();

    TaskParams(int gaPopulationLen_, int gaIterationsLen_, int grPopulationLen_,
            double saTemperature_, double saMinTemperature_, double saLambda_,
            int saInitSolution_, double acoFi_, double acoAlpha_, double acoBeta_,
            double acoQ0_, int acoPopulationLen_, int acoIterationsLen_) :
        gaPopulationLen(gaPopulationLen_), gaIterationsLen(gaPopulationLen_),
        grPopulationLen(grPopulationLen_), saTemperature(saTemperature_),
        saMinTemperature(saMinTemperature_), saLambda(saLambda_),
        saInitSolution(saInitSolution_), acoFi(acoFi_), acoAlpha(acoAlpha_),
        acoBeta(acoBeta_), acoQ0(acoQ0_), acoPopulationLen(acoPopulationLen_),
        acoIterationsLen(acoIterationsLen_) {
    }
};


struct TaskData {

    int storageLen;
    std::vector<Storage> storages;
    int userLen;
    std::vector<User> users;
    int vehicleCapacity;
    int vehicleCost;
    TaskParams taskParams;

    TaskData(int storageLen_, std::vector<Storage> storages_, int userLen_,
            std::vector<User> users_, int vehicleCapacity_, int vehicleCost_,
            TaskParams taskParams_) :
        storageLen(storageLen_), storages(storages_), userLen(userLen_),
        users(users_), vehicleCapacity(vehicleCapacity_),
        vehicleCost(vehicleCost_), taskParams(taskParams_) {
    }
};

extern void taskDataCreate(TaskData** taskData, int argc, char* argv[]);

extern void taskDataDelete(TaskData* taskData);

extern void printSolution(const Solution* sol, char* outputFile);

extern void printParams(TaskParams* taskParams);

template<class T1, class T2>
extern int euclideanDistance(T1& a, T2& b) {
    double distance = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    return (int) (distance * 100);
}

#endif // UTILS_HPP