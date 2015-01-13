#ifndef SOL_HPP
#define SOL_HPP

#include <vector>

struct Solution {
    int cost;
    int numPaths;
    std::vector<std::vector<int> > cycles;

    Solution() : cost(0), numPaths(0) {

    };

    void addCycle(std::vector<int>& v, int cycleCost);
};

#endif // SOL_HPP