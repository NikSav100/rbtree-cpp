#include "Node.h"

template <typename T>
Node<T>::Node(T value, Node<T> *parent, Node<T> *left, Node<T> *right){
    this->value = value;
    this->parent = parent;
    this->left = left;
    this->right = right;
    this->color = NONE;
}

template <typename T>
T Node<T>::get_value() { return this->value; }
template <typename T>
Node<T>* Node<T>::get_parent() { return this->parent; }
template <typename T>
Node<T>* Node<T>::get_left() { return this->left; }
template <typename T>
Node<T>* Node<T>::get_right() { return this->right; }
template <typename T>
COLORS Node<T>::get_color() { return this->color; }

template <typename T>
void Node<T>::set_parent(Node<T> *parent) {this->parent = parent; }
template <typename T>
void Node<T>::set_left(Node<T> *left) { this->left = left; }
template <typename T>
void Node<T>::set_right(Node<T> *right) { this->right = right; }
template <typename T>
void Node<T>::set_value(T value) { this->value = value; }
template <typename T>
void Node<T>::set_color(COLORS color) {this->color = color;}

template <typename T>
int Node<T>::count_children() {
    int c = 0;
    if (this->get_left()) c++;
    if (this->get_right()) c++;
    return c;
}
