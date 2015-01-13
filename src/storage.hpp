#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "node.hpp"

struct Storage : Node {

    int capacity;
    int cost;

    Storage(int id_, int x_, int y_, int capacity_, int cost_) :
       Node(id_, x_, y_), capacity(capacity_), cost(cost_) {
    }
};

#endif // STORAGE_HPP