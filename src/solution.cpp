#include <vector>

using namespace std;

#include "solution.hpp"

void Solution::addCycle(vector<int>& v, int cycleCost) {
    numPaths++;
    cost += cycleCost;
    cycles.push_back(v);
}