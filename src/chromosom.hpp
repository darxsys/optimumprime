#ifndef CHROM_HPP
#define CHROM_HPP

#include <math.h>
#include <vector>
#include <limits.h>
#include <functional>

#include "utils.hpp"

struct Chromosom {
    std::vector<int> representation;
    int openStorages;
    double cost;
    std::vector<int> capacity;
    bool falseFlag;

    Chromosom(int, int);
    Chromosom(Chromosom&, Chromosom&, int, int, int, int);
    void calculateCost(std::vector<User>&, std::vector<Storage*>&);
    void mutation();
};

bool compareChrom(const Chromosom& lhs, const Chromosom& rhs);

#endif // CHROM_HPP
