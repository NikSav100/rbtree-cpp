#include "RBTree.h"
#include <iostream>

template <typename T>
RBTree<T>::RBTree() { this->root = NULL; }

template <typename T>
RBTree<T>::RBTree(T value) {
    this->root = new Node<T>(value, NULL, NULL, NULL);
    (*this->root).set_color(BLACK);
}

//constructor for deep copy
template <typename T>
RBTree<T>::RBTree(const RBTree<T>& other):Tree<T>(){
    this->root = this->clone(other.root, NULL);
}

template <typename T>
Node<T>* RBTree<T>::clone(Node<T> *n, Node<T> *parent) {
    if (!n) return NULL;
    Node<T> *copy = new Node<T>((*n).get_value(), parent, NULL, NULL);
    (*copy).set_left(clone((*n).get_left(), copy));
    (*copy).set_right(clone((*n).get_right(), copy));
    (*copy).set_color((*n).get_color());
    return copy;
}

template <typename T>
RBTree<T>& RBTree<T>::operator=(const RBTree<T>& other) {
    if (this == &other) return *this;

    while (this->root) this->remove((*this->root).get_value());  // libero il mio vecchio contenuto

    this->root = this->clone(other.root, NULL);
    return *this;
}

//private
template <typename T>
void RBTree<T>::rotate_left(Node<T> *node) {
    if (!node) return;
    Node<T>* right = (*node).get_right();
    if (!right) return;
    Node<T> *parent = (*node).get_parent();

    //logica per la promozione del figlio destro a padre
    if (parent) {
        if ((*parent).get_right() == node) (*parent).set_right(right);
        else (*parent).set_left(right);
        (*right).set_parent(parent);
    } else {
        this->root = right;
        (*right).set_parent(NULL);
    }

    //ci prendiamo figlio sinistro di figlio destro di nodo da ruotare
    Node<T> *node_right = (*right).get_left();
    //nodo diventa figlio sinistro di quello che era suo figlio destro
    (*right).set_left(node);
    (*node).set_parent(right);
    (*node).set_right(node_right);
    if (node_right) (*node_right).set_parent(node);
    
    
    
}

//private
template <typename T>
void RBTree<T>::rotate_right(Node<T> *node) {
    if (!node) return;
    Node<T> *left = (*node).get_left();
    if (!left) return;
    Node<T> *parent = (*node).get_parent();

    //logica per la promozione del figlio sinistro a padre
    if (parent) {
        if ((*parent).get_right() == node) (*parent).set_right(left);
        else (*parent).set_left(left);
        (*left).set_parent(parent);
    } else {
        this->root = left;
        (*left).set_parent(NULL);
    }

    //ci prendiamo figlio destro di figlio sinistro di nodo da ruotare
    Node<T> *node_left = (*left).get_right();
    //nodo diventa figlio destra di quello che era suo figlio sinistro
    (*left).set_right(node);
    (*node).set_parent(left);
    (*node).set_left(node_left);
    if (node_left) (*node_left).set_parent(node);
}

//private
template <typename T>
void RBTree<T>::define_colors(Node<T>* inserted) {

    //CASE: inserted node is root
    if (this->root == inserted) {
        (*inserted).set_color(BLACK);
        return;
    }

    

    Node<T> *prnt = (*inserted).get_parent(); //parent of inserted node
    if ((*prnt).get_color() == BLACK) return;  //CASE: inserted node has black father

    //CASE: inserted node has a red father (following cases derives from this case)

    Node<T> *grndprnt = (*prnt).get_parent(); //grandfather of inserted node
    Node<T> *prnt_brthr = (*grndprnt).get_left() == prnt ? (*grndprnt).get_right() : (*grndprnt).get_left();

    //CASE: inserted node has red uncle
    if (prnt_brthr && (*prnt_brthr).get_color() == RED) {
         (*prnt).set_color(BLACK);
         (*prnt_brthr).set_color(BLACK);
         (*grndprnt).set_color(RED);
         define_colors(grndprnt);
         return;
    }

    //CASE: inserted has a black uncle or nil uncle (following cases derives from this case)
    bool done = false;
    //CASE: PARENT CHILD LEFT, INSERTED CHILD LEFT (LL)
    if (prnt == (*grndprnt).get_left() && inserted == (*prnt).get_left()) {
        this->rotate_right(grndprnt);
        done = true;
    }

    //CASE: PARENT CHILD RIGHT, INSERTED CHILD RIGHT (RR)
    if (prnt == (*grndprnt).get_right() && inserted == (*prnt).get_right()){
        this->rotate_left(grndprnt);
        done = true;
    }

    if (done) {
        (*prnt).set_color(BLACK);
        (*grndprnt).set_color(RED);
        return;
    }

    //CASE: PARENT CHILD LEFT, INSERTED CHILD RIGHT (LR)
    if (prnt == (*grndprnt).get_left() && inserted == (*prnt).get_right()) {
        this->rotate_left(prnt);
        this->rotate_right(grndprnt);
    }

    //CASE: PARENT CHILD RIGHT, INSERTED CHILD LEFT (RL)
    if (prnt == (*grndprnt).get_right() && inserted == (*prnt).get_left()) {
        this->rotate_right(prnt);
        this->rotate_left(grndprnt);
    }

    (*inserted).set_color(BLACK);
    (*grndprnt).set_color(RED);

    

    
    

    
}

template <typename T>
Node<T>* RBTree<T>::insert(T value) {

    Node<T>* node = this->Tree<T>::insert(value);
    if (node) {
        (*node).set_color(RED);
        define_colors(node);
    }

    return node;
}

template <typename T>
void RBTree<T>::fixup(Node<T> *replacement, Node<T> *replacement_parent, bool replacement_is_left) {

    Node<T> *w, *p = replacement_parent; //w -> brother of replacement, p -> father of replacement

    //fixup finché doppio nero non è stato risolto, o si è arrivati alla radice
    while (replacement != this->root && p) {
        if (replacement_is_left) {
            w = (*p).get_right();
            //CASO 1
            if (w && (*w).get_color() == RED) {
                (*w).set_color(BLACK);
                (*p).set_color(RED);
                this->rotate_left(p);
                w = (*p).get_right();
            }

            COLORS w_color, w_left_color, w_right_color;
            w_color = w_left_color = w_right_color = NONE;
            Node<T> *w_children[2] = {NULL, NULL};
            if (w) {
                w_color = (*w).get_color();
                w_children[0] = (*w).get_left();
                w_children[1] = (*w).get_right();
                w_left_color = 
                    w_children[0] ? (*w_children[0]).get_color() : BLACK,
                w_right_color = 
                    w_children[1] ? (*w_children[1]).get_color() : BLACK;
            }

            //CASO 2
            if (!w || (w_color == BLACK && w_left_color == BLACK && w_right_color == BLACK)) {
                if (w) (*w).set_color(RED);
                if ((*p).get_color() == RED) { 
                    (*p).set_color(BLACK);
                    return;
                } else {
                    replacement = p;
                    p = (*replacement).get_parent();
                    replacement_is_left = p && replacement == (*p).get_left();
                    continue;
                }
            }

            //CASO 3
            if (w_color == BLACK && w_left_color == RED && w_right_color == BLACK) {
                (*w_children[0]).set_color(BLACK);
                (*w).set_color(RED);
                this->rotate_right(w);
                w = (*p).get_right();
                for (int i = 0; i < 2; i++) w_children[i] = NULL;
                w_color = w_left_color = w_right_color = NONE;
                if (w) {
                    w_color = (*w).get_color();
                    w_children[0] = (*w).get_left();
                    w_children[1] = (*w).get_right();
                    w_left_color = 
                        w_children[0] ? (*w_children[0]).get_color() : BLACK,
                    w_right_color = 
                        w_children[1] ? (*w_children[1]).get_color() : BLACK;
                }
            }

          

            //CASO 4
            if (w && w_color == BLACK && w_right_color == RED) {
                (*w_children[1]).set_color(BLACK);
                (*w).set_color((*p).get_color());
                (*p).set_color(BLACK);
                this->rotate_left(p);
                return;
            }
            
        }
        else {
            w = (*p).get_left();
            //CASO 1
            if (w && (*w).get_color() == RED) {
                (*w).set_color(BLACK);
                (*p).set_color(RED);
                this->rotate_right(p);
                w = (*p).get_left();
            }
            
            COLORS w_color, w_left_color, w_right_color;
            w_color = w_left_color = w_right_color = NONE;
            Node<T> *w_children[2] = {NULL, NULL};

            if (w) {
                w_color = (*w).get_color();
                w_children[0] = (*w).get_left();
                w_children[1] = (*w).get_right();
                w_left_color = 
                    w_children[0] ? (*w_children[0]).get_color() : BLACK,
                w_right_color = 
                    w_children[1] ? (*w_children[1]).get_color() : BLACK;
            }

            //CASO 2
            if (!w || (w_color == BLACK && w_left_color == BLACK && w_right_color == BLACK)) {
                if (w) (*w).set_color(RED);
                if ((*p).get_color() == RED) { 
                    (*p).set_color(BLACK);
                    return;
                } else {
                    replacement = p;
                    p = (*replacement).get_parent();
                    replacement_is_left = p && replacement == (*p).get_left();
                    continue;
                }
            }

            //CASO 3
            if (w_color == BLACK && w_right_color == RED && w_left_color == BLACK) {
                (*w_children[1]).set_color(BLACK);
                (*w).set_color(RED);
                this->rotate_left(w);
                w = (*p).get_left();
                for (int i = 0; i < 2; i++) w_children[i] = NULL;
                w_color = w_left_color = w_right_color = NONE;
                if (w) {
                    w_color = (*w).get_color();
                    w_children[0] = (*w).get_left();
                    w_children[1] = (*w).get_right();
                    w_left_color = 
                        w_children[0] ? (*w_children[0]).get_color() : BLACK,
                    w_right_color = 
                        w_children[1] ? (*w_children[1]).get_color() : BLACK;
                }
            }

            //CASO 4
            if (w && w_color == BLACK && w_left_color == RED) {
                (*w_children[0]).set_color(BLACK);
                (*w).set_color((*p).get_color());
                (*p).set_color(BLACK);
                this->rotate_right(p);
                return;
            }
        }
    }

}

template <typename T>
Node<T>* RBTree<T>::remove(T value) {

    Node<T> *x = this->lookup(value);
    if (!x) return NULL;

    Node<T> *x_parent = (*x).get_parent();
    int x_children = (*x).count_children();
    COLORS x_color = (*x).get_color();
    
    Node<T> *successor, *successor_parent;
    Node<T> *replacement, *replacement_parent;
    successor = successor_parent = NULL;

    bool replacement_is_left;

    if (x_children == 2) {
        successor = this->Tree<T>::predecessor(x);
        successor_parent = (*successor).get_parent();
    }

    COLORS original_color = x_children < 2 ? x_color : (*successor).get_color();
    if (x_children == 2) (*successor).set_color(x_color);

    if (x_children < 2) {
        replacement_parent = x_parent;
        replacement_is_left =
            x_parent && (*x_parent).get_left() == x;
    } else {
        replacement_parent =
            successor_parent != x ? successor_parent : successor;
    
        replacement_is_left = successor_parent == x;
    }

    replacement = this->Tree<T>::remove((*x).get_value());

    if (original_color == RED) {
        if (replacement) (*replacement).set_color(BLACK);
        return replacement;
    }

    if (replacement && (*replacement).get_color() == RED) {
        (*replacement).set_color(BLACK);
        return replacement;
    }
 

    this->fixup(replacement, replacement_parent, replacement_is_left);

    return replacement;
    
}

