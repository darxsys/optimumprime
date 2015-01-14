#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "utils.hpp"

#define BUFFER_SIZE 1024

using namespace std;

static struct option options[] = {
    {"input", required_argument, 0, 'i'},
    {"ga-population", required_argument, 0, 'a'},
    {"ga-iterations", required_argument, 0, 'b'},
    {"gr-population", required_argument, 0, 'c'},
    {"sa-temperature", required_argument, 0, 'd'},
    {"sa-min-temperature", required_argument, 0, 'e'},
    {"sa-lambda", required_argument, 0, 'f'},
    {"sa-init-solution", required_argument, 0, 'g'},
    {"aco-fi", required_argument, 0, 'j'},
    {"aco-alpha", required_argument, 0, 'k'},
    {"aco-beta", required_argument, 0, 'l'},
    {"aco-q0", required_argument, 0, 'm'},
    {"aco-population", required_argument, 0, 'n'},
    {"aco-iterations", required_argument, 0, 'o'},
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

static void parseArguments(char** inputPath, TaskParams** taskParams, int argc,
    char* argv[]);

static void parseFile(int* storageLen, vector<pair<int, int> >& storageCoordinates,
    vector<int>& storageCapacity, vector<int>& storageCost, int* userLen,
    vector<pair<int, int> >& userCoordinates, vector<int>& userDemand,
    int* vehicleCapacity, int* vehicleCost, char* inputPath);

static void help();

// ***************************************************************************



// ***************************************************************************
// PUBLIC

extern void taskDataCreate(TaskData** taskData, int argc, char* argv[]) {

    char* inputPath = NULL;
    TaskParams* taskParams = NULL;

    parseArguments(&inputPath, &taskParams, argc, argv);

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

extern void printParams(TaskParams& taskParams) {
    fprintf(stderr, "#####################################\n");
    fprintf(stderr, "[Parameters]\n");
    fprintf(stderr, " [Grouping] Genetic algorithm\n");
    fprintf(stderr, "%-20s %15d\n", "  -population", taskParams.gaPopulationLen);
    fprintf(stderr, "%-20s %15d\n", "  -iterations", taskParams.gaIterationsLen);
    fprintf(stderr, " [Grouping] Greedy algorithm\n");
    fprintf(stderr, "%-20s %15d\n", "  -population", taskParams.grPopulationLen);
    fprintf(stderr, " [Grouping] Simulated annealing\n");
    fprintf(stderr, "%-20s %15.3lf\n", "  -temperature", taskParams.saTemperature);
    fprintf(stderr, "%-20s %15.3lf\n", "  -min temperature", taskParams.saMinTemperature);
    fprintf(stderr, "%-20s %15.3lf\n", "  -lambda", taskParams.saLambda);
    fprintf(stderr, "%-20s %15d\n", "  -init solution",taskParams.saInitSolution);
    fprintf(stderr, " [TSP] Ant colony optimization\n");
    fprintf(stderr, "%-20s %15.3lf\n", "  -fi", taskParams.acoFi);
    fprintf(stderr, "%-20s %15.3lf\n", "  -alpha", taskParams.acoAlpha);
    fprintf(stderr, "%-20s %15.3lf\n", "  -beta", taskParams.acoBeta);
    fprintf(stderr, "%-20s %15.3lf\n", "  -q0", taskParams.acoQ0);
    fprintf(stderr, "%-20s %15d\n", "  -population", taskParams.acoPopulationLen);
    fprintf(stderr, "%-20s %15d\n", "  -iterations", taskParams.acoIterationsLen);
    fprintf(stderr, "#####################################\n");
}

// ***************************************************************************

// ***************************************************************************
// PRIVATE

static void parseArguments(char** inputPath, TaskParams** taskParams, int argc,
    char* argv[]) {

    int gaPopulationLen = 50;
    int gaIterationsLen = 75000;

    int grPopulationLen = 50;

    double saTemperature = 200;
    double saMinTemperature = 0.01;
    double saLambda = 0.99;
    int saInitSolution = 1;

    double acoFi = 0.3;
    double acoAlpha = 1.0;
    double acoBeta = 2.0;
    double acoQ0 = 0.1;
    int acoPopulationLen = -1;
    int acoIterationsLen = 5000;

    while (1) {

        char argument = getopt_long(argc, argv, "i:h", options, NULL);

        if (argument == -1) {
            break;
        }

        switch (argument) {
        case 'i':
            *inputPath = optarg;
            break;
        case 'a':
            gaPopulationLen = atoi(optarg);
            break;
        case 'b':
            gaIterationsLen = atoi(optarg);
            break;
        case 'c':
            grPopulationLen = atoi(optarg);
            break;
        case 'd':
            saTemperature = atof(optarg);
            break;
        case 'e':
            saMinTemperature = atof(optarg);
            break;
        case 'f':
            saLambda = atof(optarg);
            break;
        case 'g':
            saInitSolution = atoi(optarg);
            break;
        case 'j':
            acoFi = atof(optarg);
            break;
        case 'k':
            acoAlpha = atof(optarg);
            break;
        case 'l':
            acoBeta = atof(optarg);
            break;
        case 'm':
            acoQ0 = atof(optarg);
            break;
        case 'n':
            acoPopulationLen = atoi(optarg);
            break;
        case 'o':
            acoIterationsLen = atoi(optarg);
            break;
        default:
            help();
            exit(-1);
        }
    }

    ASSERT(*inputPath != NULL, "missing option -i");

    ASSERT(gaPopulationLen % 2 == 0 && gaPopulationLen > 0, "invalid GA population");
    ASSERT(gaIterationsLen > 0, "invalid GA iterations");

    ASSERT(grPopulationLen > 0, "invalid GR population");

    ASSERT(saTemperature > 0, "invalide SA start temperature");
    ASSERT(saMinTemperature > 0, "invalide SA stop temperature");
    ASSERT(saLambda >= 0 && saLambda <= 1, "invalid SA lambda");
    ASSERT(saInitSolution == 0 || saInitSolution == 1 || saInitSolution == 2,
        "invalid SA init solution index");

    ASSERT(acoFi >= 0 && acoFi <= 1, "invalid ACO fi");
    ASSERT(acoAlpha >= 0, "invalid ACO alpha");
    ASSERT(acoBeta >= 0, "invalid ACO beta");
    ASSERT(acoQ0 >= 0 && acoQ0 <= 1, "invalid ACO q0");
    ASSERT(acoPopulationLen == -1 || acoPopulationLen > 0, "invalid ACO population");
    ASSERT(acoIterationsLen > 0, "invalid ACO iterations");

    *taskParams = new TaskParams(gaPopulationLen, gaIterationsLen, grPopulationLen,
        saTemperature, saMinTemperature, saLambda, saInitSolution, acoFi, acoAlpha,
        acoBeta, acoQ0, acoPopulationLen, acoIterationsLen);
}

static void parseFile(int* storageLen, vector<pair<int, int> >& storageCoordinates,
    vector<int>& storageCapacity, vector<int>& storageCost, int* userLen,
    vector<pair<int, int> >& userCoordinates, vector<int>& userDemand,
    int* vehicleCapacity, int* vehicleCost, char* inputPath) {

    FILE* inputFile = fopen(inputPath, "r");

    char* buffer = new char[BUFFER_SIZE];
    char* garbage = new char[BUFFER_SIZE];
    char* err = NULL;

    err = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", userLen, garbage);

    err = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", storageLen, garbage);

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    userCoordinates.reserve(*userLen);
    userDemand.reserve(*userLen);

    storageCoordinates.reserve(*storageLen);
    storageCapacity.reserve(*storageLen);
    storageCost.reserve(*storageLen);

    for (int i = 0; i < *storageLen; ++i) {
        int x, y;

        err = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d\t%d%s", &x, &y, garbage);

        storageCoordinates.emplace_back(x, y);
    }

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *userLen; ++i) {
        int x, y;

        err = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d\t%d%s", &x, &y, garbage);

        userCoordinates.emplace_back(x, y);
    }

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    err = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", vehicleCapacity, garbage);

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *storageLen; ++i) {
        int capacity;

        err = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &capacity, garbage);

        storageCapacity.emplace_back(capacity);
    }

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *userLen; ++i) {
        int demand;

        err = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &demand, garbage);

        userDemand.emplace_back(demand);
    }

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    for (int i = 0; i < *storageLen; ++i) {
        int cost;

        err = fgets(buffer, BUFFER_SIZE, inputFile);
        sscanf(buffer, "%d%s", &cost, garbage);

        storageCost.emplace_back(cost);
    }

    err = fgets(buffer, BUFFER_SIZE, inputFile);

    err = fgets(buffer, BUFFER_SIZE, inputFile);
    sscanf(buffer, "%d%s", vehicleCost, garbage);

    if (err != NULL) { 
        // free(err);
    }

    delete[] garbage;
    delete[] buffer;

    fclose(inputFile);
}

static void help() {
    printf(
    "usage: optimumprime -i <input file> [arguments ...]\n"
    "\n"
    "arguments:\n"
    "    -i, --input <file>\n"
    "        (required)\n"
    "        input file\n"
    "    [Grouping] Genetic algorithm\n"
    "        --ga-population <int>\n"
    "            default: 50\n"
    "        --ga-iterations <int>\n"
    "            default: 75000\n"
    "    [Grouping] Greedy algorithm\n"
    "        --gr-population <int>\n"
    "            default: 50\n"
    "    [Grouping] Simulated annealing\n"
    "         --sa-temperature <double>\n"
    "             default: 200.0\n"
    "         --sa-min-temperature <double>\n"
    "             default: 0.01\n"
    "         --sa-lambda <double>\n"
    "             default: 0.99\n"
    "         --sa-init-solution <int>\n"
    "             default: 1\n"
    "             0 - random, 1 - greedy, 2 - genetic\n"
    "    [TSP] Ant colony optimization\n"
    "         --aco-fi <double>\n"
    "             default: 0.3\n"
    "         --aco-alpha <double>\n"
    "             default: 1.0\n"
    "         --aco-beta <double>\n"
    "             default: 2.0\n"
    "         --aco-q0 <double>\n"
    "             default: 0.1\n"
    "         --aco-population <int>\n"
    "             default: -1\n"
    "             -1 - number of nodes in graph\n"
    "         --aco-iterations <int>\n"
    "             default: 5000\n"
    "    -h, -help\n"
    "        prints out the help\n");
}

// ***************************************************************************