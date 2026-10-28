#ifndef NODE_H
#define NODE_H

enum COLORS {
    NONE = -1,
    BLACK = 0, //0
    RED = 1 //1
};

template <typename T>
class Node{    
    private:
        T value;
        Node<T> *parent;
        Node<T> *left; //left child
        Node<T> *right; //right child
        COLORS color;
    public:
        Node(T, Node<T>*, Node<T>*, Node<T>*);

         T get_value();
         Node<T>* get_parent();
         Node<T>* get_left();
         Node<T>* get_right();
         COLORS get_color();

        void set_parent(Node<T>*);
        void set_left(Node<T>*);
        void set_right(Node<T>*);
        void set_value(T);
        void set_color(COLORS);

        int count_children();
};

#include "Node.tpp"

#endif