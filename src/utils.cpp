#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "utils.hpp"

#define BUFFER_SIZE 1024

using namespace std;

static struct option options[] = {
    {"input", required_argument, 0, 'i'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

// ***************************************************************************
// PUBLIC

extern void taskDataCreate(TaskData** taskData, int argc, char* argv[]);

extern void taskDataDelete(TaskData* taskData);

extern void printSolution(const Solution* sol, char* outputFile);

extern void printParams(TaskParams* taskParams);

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void parseArguments(char* inputPath, TaskParams** taskParams, int argc,
    char* argv[]);

static void help();

static void parseFile(int* storageLen, vector<pair<int, int> >& storageCoordinates,
    vector<int>& storageCapacity, vector<int>& storageCost, int* userLen,
    vector<pair<int, int> >& userCoordinates, vector<int>& userDemand,
    int* vehicleCapacity, int* vehicleCost, char* inputPath);

// ***************************************************************************



// ***************************************************************************
// PUBLIC

extern void taskDataCreate(TaskData** taskData, int argc, char* argv[]) {

    char* inputPath = NULL;
    TaskParams* taskParams = NULL;

    parseArguments(inputPath, &taskParams, argc, argv);

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
        storages.emplace_back(i, storageCoordinates[i].first, storageCoordinates[i].second,
            storageCapacity[i], storageCost[i]);
    }

    for (int i = 0; i < userLen; ++i) {
        users.emplace_back(i, userCoordinates[i].first, userCoordinates[i].second,
            userDemand[i]);
    }

    *taskData = new TaskData(storageLen, storages, userLen, users,
        vehicleCapacity, vehicleCost, *taskParams);

    delete taskParams;
}

extern void taskDataDelete(TaskData* taskData) {
    delete taskData;
}

extern void printSolution(const Solution* sol, char* outputFile) {

    FILE *out = fopen(outputFile, "w");
    ASSERT(out != NULL, "Can't open output file.");

    fprintf(out, "%d\n", sol->numPaths);
    for (int i = 0; i < sol->numPaths; ++i) {
        for (int j = 0; j < (int) sol->cycles[i].size(); ++j) {
            fprintf(out, "%d ", sol->cycles[i][j]);
        }

        fprintf(out, "\n");
    }

    fprintf(out, "%d\n", sol->cost);

    fclose(out);
}

extern void printParams(TaskParams* taskParams) {
    fprintf(stderr, "Parameters\n");
    fprintf(stderr, "[Preproc] Genetic algorithm\n");
    fprintf(stderr, "  population       %d\n", taskParams->gaPopulationLen);
    fprintf(stderr, "  iterations       %d\n", taskParams->gaIterationsLen);
    fprintf(stderr, "[Preproc] Greedy algorithm\n");
    fprintf(stderr, "  population       %d\n", taskParams->grPopulationLen);
    fprintf(stderr, "[Preproc] Simulated annealing\n");
    fprintf(stderr, "  temperature     %lf\n", taskParams->saTemperature);
    fprintf(stderr, "  min temperature %lf\n", taskParams->saMinTemperature);
    fprintf(stderr, "  lambda          %lf\n", taskParams->saLambda);
    fprintf(stderr, "  init solution    %d\n", taskParams->saInitSolution);
    fprintf(stderr, "[Solver] Ant colony optimization\n");
    fprintf(stderr, "  fi              %lf\n", taskParams->acoFi);
    fprintf(stderr, "  alpha           %lf\n", taskParams->acoAlpha);
    fprintf(stderr, "  beta            %lf\n", taskParams->acoBeta);
    fprintf(stderr, "  q0              %lf\n", taskParams->acoQ0);
    fprintf(stderr, "  population       %d\n", taskParams->acoPopulationLen);
    fprintf(stderr, "  iterations       %d\n", taskParams->acoIterationsLen);
}

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void parseArguments(char* inputPath, TaskParams** taskParams, int argc,
    char* argv[]) {

     while (1) {

        char argument = getopt_long(argc, argv, "i:h", options, NULL);

        if (argument == -1) {
            break;
        }

        switch (argument) {
        case 'i':
            inputPath = optarg;
            break;
        default:
            help();
            exit(-1);
        }
    }

    ASSERT(inputPath != NULL, "missing option -i");
}

static void help() {
    printf(
    "usage: optimumprime -i <input file> [arguments ...]\n"
    "\n"
    "arguments:\n"
    "    -i, --input <file>\n"
    "        (required)\n"
    "        input file\n"
    "    -h, -help\n"
    "        prints out the help\n");
}

static void parseFile(int* storageLen, vector<pair<int, int> >& storageCoordinates,
    vector<int>& storageCapacity, vector<int>& storageCost, int* userLen,
    vector<pair<int, int> >& userCoordinates, vector<int>& userDemand,
    int* vehicleCapacity, int* vehicleCost, char* inputPath) {

    FILE* inputFile = fopen(inputPath, "r");

    char* buffer = new char[BUFFER_SIZE];
    char* garbage = new char[BUFFER_SIZE];

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", userLen, garbage);

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", storageLen, garbage);

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    userCoordinates.reserve(*userLen);
    userDemand.reserve(*userLen);

    storageCoordinates.reserve(*storageLen);
    storageCapacity.reserve(*storageLen);
    storageCost.reserve(*storageLen);

    for (int i = 0; i < *storageLen; ++i) {
        int x, y;

        garbage = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d\t%d%s", &x, &y, garbage);

        storageCoordinates.emplace_back(x, y);
    }

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *userLen; ++i) {
        int x, y;

        garbage = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d\t%d%s", &x, &y, garbage);

        userCoordinates.emplace_back(x, y);
    }

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", vehicleCapacity, garbage);

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *storageLen; ++i) {
        int capacity;

        garbage = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &capacity, garbage);

        storageCapacity.emplace_back(capacity);
    }

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *userLen; ++i) {
        int demand;

        garbage = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &demand, garbage);

        userDemand.emplace_back(demand);
    }

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *storageLen; ++i) {
        int cost;

        garbage = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &cost, garbage);

        storageCost.emplace_back(cost);
    }

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);

    garbage = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", vehicleCost, garbage);

    delete[] garbage;
    delete[] buffer;

    fclose(inputFile);
}

// ***************************************************************************