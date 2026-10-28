#ifndef TREE_H
#define TREE_H

#include "Node.h"

template <typename T>
class Tree {
    protected:
        Node<T> *root;
        Node<T>* predecessor(Node<T>*);
        virtual Node<T>* clone(Node<T>*, Node<T>*); // method for deep copy constructor
    public:
        Tree();
        Tree(T value);
        virtual ~Tree();
        Tree(const Tree& other); //constructor for deep copy
        virtual Tree& operator=(const Tree& other);  //when = is used, deep copy constructor is called
        virtual Node<T>* insert(T value);
        virtual Node<T>* remove(T value);
        Node<T>* lookup(T value);
        void print();



};

#include "Tree.tpp"

#endif
