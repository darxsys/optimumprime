#ifndef USER_HPP
#define USER_HPP

#include "node.hpp"

struct User : Node {

    int demand;

    User(int id_, int x_, int y_, int demand_) :
        Node(id_, x_, y_), demand(demand_) {
    }
};

#endif // USER_HPP