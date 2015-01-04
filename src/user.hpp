#ifndef USER_HPP
#define USER_HPP

#pragma once

struct User {

    int x;
    int y;
    int demand;

    User(int x_, int y_, int demand_) :
        x(x_), y(y_), demand(demand_) {
    }
};

#endif // USER_HPP