#ifndef RBTREE_H
#define RBTREE_H

#include "Tree.h"

template <typename T>
class RBTree: public Tree<T>{
    private:
        Node<T>* clone(Node<T>*, Node<T>*); // method for deep copy constructor
        void define_colors(Node<T>*);
        void rotate_left(Node<T>*);
        void rotate_right(Node<T>*);
        void fixup(Node<T>*, Node<T>*, bool);
    public:
        RBTree(); //default constructor
        RBTree(T value); //custom constructor 
        RBTree(const RBTree& other); //constructor for deep copy
        RBTree& operator=(const RBTree& other);  //when = is used, deep copy constructor is called
        Node<T>* insert(T value);
        Node<T>* remove(T value);
};

#include "RBTree.tpp"

#endif