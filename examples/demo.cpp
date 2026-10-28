#include <iostream>
#include <string>
#include "RBTree.h"

int main() {
    RBTree<int> t;
    for (int v = 1; v <= 10; v++) t.insert(v);   // inserimento ordinato: il caso peggiore per un BST semplice

    std::cout << "Albero dopo aver inserito 1..10:\n";
    t.print();

    std::cout << "\n\nInserisco un duplicato (5): "
              << (t.insert(5) ? "inserito" : "ignorato") << "\n";

    std::cout << "Cerco 7: " << (t.lookup(7) ? "trovato" : "assente") << "\n";
    t.remove(7);
    std::cout << "Dopo remove(7), cerco 7: " << (t.lookup(7) ? "trovato" : "assente") << "\n";

    RBTree<int> copia = t;                        // copia profonda (valori e colori)
    copia.remove(1);
    std::cout << "Copia indipendente: nella copia 1 e' " << (copia.lookup(1) ? "presente" : "rimosso")
              << ", nell'originale e' " << (t.lookup(1) ? "presente" : "rimosso") << "\n";

    RBTree<std::string> frutta;
    frutta.insert("mela"); frutta.insert("pera"); frutta.insert("kiwi");
    std::cout << "\nAlbero di stringhe:\n";
    frutta.print();
    std::cout << std::endl;
    return 0;
}
