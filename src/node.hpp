#ifndef NODE_HPP
#define NODE_HPP
    
struct Node {

    int id;
    int x;
    int y;
    int type; // 0 client 1 storage

    Node(int id_, int x_, int y_) :
        id(id_), x(x_), y(y_) {

        };

    virtual ~Node() = 0; // prevent this from being instantiated
};

#endif // NODE_HPP