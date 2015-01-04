#ifndef STORAGE_HPP
#define STORAGE_HPP

struct Storage {

    int x;
    int y;
    int capacity;
    int cost;

    Storage(int x_, int y_, int capacity_, int cost_) :
        x(x_), y(y_), capacity(capacity_), cost(cost_) {
    }
};

#endif // STORAGE_HPP