#include <iostream>
#include <queue>
#include "Tree.h"

//PUBLIC METHODS
template <typename T>
Tree<T>::Tree() : root(NULL) {}

template <typename T>
Tree<T>::Tree(T value){
    this->root = new Node<T>(value, NULL, NULL, NULL);
}

template <typename T>
Tree<T>::Tree(const Tree<T>& other) {
    this->root = clone(other.root, NULL);
}

template <typename T>
Node<T>* Tree<T>::clone(Node<T> *n, Node<T> *parent) {
    if (!n) return NULL;
    Node<T> *copy = new Node<T>((*n).get_value(), parent, NULL, NULL);
    (*copy).set_left(clone((*n).get_left(), copy));
    (*copy).set_right(clone((*n).get_right(), copy));
    return copy;
}

template <typename T>
Tree<T>& Tree<T>::operator=(const Tree<T>& other) {
    if (this == &other) return *this;             // self-assignment: c = c

    while (this->root) remove((*this->root).get_value());  // libero il mio vecchio contenuto

    this->root = clone(other.root, NULL);
    return *this;
}


template <typename T>
Tree<T>::~Tree() {
    while (this->root) remove((*this->root).get_value());
}


template <typename T>
Node<T>* Tree<T>::insert(T value) {

    if (!this->root) {
        this->root = new Node<T>(value, NULL, NULL, NULL);
        return this->root;
    }

    Node<T> *aux = this->root;
    Node<T> *prnt;
    int where; //it marks where to insert the node (0 left, 1 right)

    while (aux) {
        prnt = aux;
        if (value > (*aux).get_value()) {
            where = 1;
            aux = (*aux).get_right();
        } else if (value < (*aux).get_value()) {
            aux = (*aux).get_left();
            where = 0;
        } else return NULL;

    }

    Node<T> *node = new Node<T>(value, prnt, NULL, NULL);

    //insert to right for parent
    if (where) (*prnt).set_right(node);
    else (*prnt).set_left(node);

    return node;

}

//return REMPLACEMENT for FIX-UP
template <typename T>
Node<T>* Tree<T>::remove(T value) {

    if (!this->root) return NULL;
        
    Node<T> *aux = this->root;
    T aux_val;
    while (aux) {
        aux_val = (*aux).get_value();
        if (value < aux_val) aux = (*aux).get_left();
        else if (value > aux_val) aux = (*aux).get_right();
        else {
            Node<T>* children[2] = {(*aux).get_left(), (*aux).get_right()};
            int children_counter = 0, j;
            for (int i = 0; i < 2; i++) { 
                if (children[i]) {
                    children_counter++;
                    j = i;
                }
            }

            if (!children_counter) {
                 Node<T> *parent = (*aux).get_parent();
                if (aux != this->root) {
                    if ((*parent).get_left() == aux) (*parent).set_left(NULL);
                    if ((*parent).get_right() == aux) (*parent).set_right(NULL);
                } else this->root = NULL;
                delete aux; //delete node with no children
                return NULL;
            } 
            else if (children_counter < 2) {
                
                Node<T> *parent = (*aux).get_parent();

                if (parent) {
                    if ((*parent).get_left() == aux) (*parent).set_left(children[j]);
                    if ((*parent).get_right() == aux) (*parent).set_right(children[j]);
                    (*children[j]).set_parent(parent);
                } else {
                    this->root = children[j];
                    (*children[j]).set_parent(NULL); 
                }

                delete aux;
                return children[j];
            } else {

                Node<T> *pred = this->predecessor(aux);
                Node<T> *pred_parent = (*pred).get_parent();
                Node<T> *aux_parent = (*aux).get_parent();
                Node<T> *l_pred = (*pred).get_left();

                (*children[1]).set_parent(pred);

                if (pred_parent != aux) {
                    if (l_pred) {
                        (*pred_parent).set_right((*pred).get_left());
                        (*l_pred).set_parent(pred_parent);
                    }
                    else (*pred_parent).set_right(NULL);
                    (*pred).set_left(children[0]);
                    (*children[0]).set_parent(pred);
                }

                (*pred).set_right(children[1]);
                if (aux_parent) {
                    if (aux == (*aux_parent).get_left()) (*aux_parent).set_left(pred);
                    if (aux == (*aux_parent).get_right()) (*aux_parent).set_right(pred);
                    (*pred).set_parent(aux_parent);
                } else {
                    this->root = pred;
                    (*pred).set_parent(NULL);
                }

                delete aux;
                return l_pred;
            }
        }
    }

    return NULL;

    
}

template <typename T>
Node<T>* Tree<T>::lookup(T value) {

    if (!this->root) return NULL;

    Node<T> *aux = this->root;

    while (aux) {
        if (value == (*aux).get_value()) return aux;
        else if (value > (*aux).get_value()) aux = (*aux).get_right();
        else aux = (*aux).get_left();
    }

    return NULL;
    
}

template <typename T>
void Tree<T>::print() {

    if (!this->root) return;

    std::queue<Node<T> *> q;
    Node<T> *aux, *lft, *rgt;
    T v_lft, v_rgt;
    std::string check_lft, check_rgt;
    std::string colors[] = {"None", "B", "R"};

    q.push(this->root);
    while (!q.empty()) {
        aux = q.front();
        q.pop();
        lft = (*aux).get_left();
        rgt = (*aux).get_right();
        check_lft = check_rgt = "";
        if (lft) {
            v_lft = (*lft).get_value();
            check_lft = "no_nil";
            q.push(lft);
        }
        if (rgt) {
            v_rgt = (*rgt).get_value();
            check_rgt = "no_nil";
            q.push(rgt);
        }
        std::cout << "\nelement: " << (*aux).get_value() << "(" + colors[(*aux).get_color() + 1] + ") ";
        std::cout <<" children: ";
        if (check_lft != "") std::cout << v_lft;
        else std::cout << "nil";
        std::cout << ", ";
        if (check_rgt != "") std::cout << v_rgt;
        else std::cout << "nil";
        std::cout << std::endl;
    }

}

//PRIVATE METHODS

template <typename T>
Node<T>* Tree<T>::predecessor(Node<T> *aux) {
    Node<T> *start = (*aux).get_left(), *res = NULL;

    while (start) {
        res = start;
        start = (*start).get_right();
    }

    return res;

}