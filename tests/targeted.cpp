#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <climits>
#include <cstdlib>
#define private public
#define protected public
#include "RBTree.h"
#undef private
#undef protected

typedef Node<int> N;

// 0 = ok, altrimenti codice del problema
static std::string valid(N* n, N* par, int& bh, bool isroot = true) {
    if (!n) { bh = 0; return ""; }
    if (n->get_parent() != par) return "parent errato";
    COLORS c = n->get_color();
    if (c != RED && c != BLACK) return "colore NONE su " + std::to_string(n->get_value());
    if (isroot && c != BLACK) return "radice non nera";
    N *l = n->get_left(), *r = n->get_right();
    if (c == RED && ((l && l->get_color() == RED) || (r && r->get_color() == RED))) return "rosso-rosso";
    int lh, rh;
    std::string e = valid(l, n, lh, false); if (!e.empty()) return e;
    e = valid(r, n, rh, false); if (!e.empty()) return e;
    if (lh != rh) return "altezza nera diversa";
    bh = lh + (c == BLACK);
    return "";
}
static std::string valid(RBTree<int>& t) { int bh; return valid(t.root, NULL, bh); }
static std::string keys(N* n) { return n ? keys(n->get_left()) + std::to_string(n->get_value()) + " " + keys(n->get_right()) : ""; }
static std::string shape(N* n) {   // forma + colori, per confrontare due alberi
    if (!n) return ".";
    return "(" + shape(n->get_left()) + std::to_string(n->get_value()) + (n->get_color() == RED ? "R" : n->get_color() == BLACK ? "B" : "?") + shape(n->get_right()) + ")";
}
static void build(RBTree<int>& t, std::initializer_list<int> v) { for (int x : v) t.insert(x); }

#define CHECK(cond, msg) do { if (!(cond)) { std::cout << "  PROBLEMA: " << msg << "\n"; bad++; } } while (0)

int main(int argc, char** argv) {
    int which = argc > 1 ? atoi(argv[1]) : 0;
    int bad = 0;

    switch (which) {
    case 1: { std::cout << "T1 costruttore di copia: stessi valori e colori, albero indipendente\n";
        RBTree<int> a; build(a, {50,30,70,20,40,60,80,10,25,35,45});
        RBTree<int> b = a;
        CHECK(shape(a.root) == shape(b.root), "forma/colori diversi: " << shape(a.root) << " vs " << shape(b.root));
        CHECK(valid(b).empty(), "copia non valida: " + valid(b));
        b.insert(99); b.remove(30);
        CHECK(a.lookup(99) == NULL, "inserire nella copia modifica l'originale");
        CHECK(a.lookup(30) != NULL, "rimuovere dalla copia modifica l'originale");
        CHECK(valid(a).empty(), "originale non piu' valido: " + valid(a));
        CHECK(valid(b).empty(), "copia non valida dopo modifiche: " + valid(b));
        break; }
    case 2: { std::cout << "T2 copia di albero vuoto e di albero con un nodo\n";
        RBTree<int> e; RBTree<int> e2 = e;
        CHECK(e2.root == NULL, "copia di vuoto non vuota");
        RBTree<int> one(5); RBTree<int> one2 = one;
        CHECK(one2.root && one2.root->get_color() == BLACK && one2.root != one.root, "copia di un nodo errata");
        break; }
    case 3: { std::cout << "T3 operator= RBTree: albero di destinazione gia' pieno (controllo perdite di memoria)\n";
        RBTree<int> a; build(a, {50,30,70,20,40});
        RBTree<int> x; build(x, {1,2,3,4,5,6,7,8,9,10});
        x = a;
        CHECK(shape(x.root) == shape(a.root), "dopo x = a l'albero e' diverso: " << shape(x.root));
        CHECK(valid(x).empty(), "x non valido: " + valid(x));
        CHECK(x.lookup(7) == NULL, "contenuto vecchio di x ancora raggiungibile");
        break; }
    case 4: { std::cout << "T4 operator= RBTree: autoassegnazione e assegnazione di vuoto\n";
        RBTree<int> a; build(a, {5,3,8,1,4});
        std::string before = shape(a.root);
        a = a;
        CHECK(shape(a.root) == before, "autoassegnazione ha cambiato l'albero");
        RBTree<int> empty; a = empty;
        CHECK(a.root == NULL, "assegnare un albero vuoto non svuota la destinazione");
        break; }
    case 5: { std::cout << "T5 operator= tramite riferimento a Tree& (Tree::operator= virtuale ma con firma diversa)\n";
        RBTree<int> a; build(a, {50,30,70,20,40,60,80});
        RBTree<int> x; build(x, {1,2,3});
        Tree<int>& rx = x; const Tree<int>& ra = a;
        rx = ra;
        CHECK(shape(x.root) == shape(a.root), "assegnazione via Tree& : " << shape(x.root) << " vs " << shape(a.root));
        CHECK(valid(x).empty(), "x non valido dopo assegnazione via base: " + valid(x));
        break; }
    case 6: { std::cout << "T6 distruzione tramite Tree* (distruttore virtuale)\n";
        Tree<int>* p = new RBTree<int>();
        for (int v : {5,3,8,1,4,7,9}) p->insert(v);
        delete p;
        break; }
    case 7: { std::cout << "T7 insert tramite Tree& (insert e' virtuale: deve mantenere le regole rosso-nero)\n";
        RBTree<int> t; Tree<int>& r = t;
        for (int v = 1; v <= 100; v++) r.insert(v);
        CHECK(valid(t).empty(), "albero non valido dopo insert via base: " + valid(t));
        break; }
    case 8: { std::cout << "T8 remove tramite Tree& (remove NON e' virtuale: salta il fix-up)\n";
        RBTree<int> t; build(t, {50,30,70,20,40,60,80,10,25,35,45,55,65,75,85});
        Tree<int>& r = t;
        for (int v : {10, 20, 25, 30}) r.remove(v);
        CHECK(valid(t).empty(), "albero non valido dopo remove via base: " + valid(t));
        break; }
    case 9: { std::cout << "T9 costruzione di un Tree da un RBTree (slicing): la copia mantiene i colori?\n";
        RBTree<int> a; build(a, {50,30,70});
        Tree<int> s = a;              // costruttore di copia di Tree, clone chiamato dentro il costruttore
        CHECK(s.root && s.root->get_color() == BLACK, "colore della radice perso nello slicing (" << (s.root ? (int)s.root->get_color() : -9) << ")");
        break; }
    case 10: { std::cout << "T10 operazioni su albero vuoto\n";
        RBTree<int> t;
        t.remove(5); t.print();
        CHECK(t.lookup(5) == NULL, "lookup su vuoto");
        t.insert(1); t.remove(1);
        CHECK(t.root == NULL, "dopo insert+remove la radice non e' nulla");
        t.remove(1);
        t.insert(2);
        CHECK(valid(t).empty(), "albero non valido dopo riuso: " + valid(t));
        break; }
    case 11: { std::cout << "T11 tipi diversi da int: std::string e double\n";
        RBTree<std::string> s;
        for (std::string w : {"mela","pera","uva","kiwi","fico","limone","arancia"}) s.insert(w);
        s.remove("pera"); s.remove("mela");
        CHECK(s.lookup("kiwi") != NULL && s.lookup("pera") == NULL, "ricerca su stringhe errata");
        RBTree<double> d; for (double x : {3.5, 1.25, 9.75, 4.0}) d.insert(x); d.remove(1.25);
        CHECK(d.lookup(4.0) != NULL, "ricerca su double errata");
        RBTree<std::string> s2 = s;
        CHECK(s2.lookup("kiwi") != NULL, "copia di RBTree<string> errata");
        break; }
    case 12: { std::cout << "T12 copia dopo molte rimozioni, poi distruzione di entrambe\n";
        RBTree<int> a; for (int i = 0; i < 300; i++) a.insert((i * 37) % 301);
        for (int i = 0; i < 300; i += 3) a.remove((i * 37) % 301);
        RBTree<int> b = a;
        CHECK(shape(a.root) == shape(b.root), "copia diversa dall'originale");
        CHECK(valid(b).empty(), "copia non valida: " + valid(b));
        break; }
    default: std::cout << "uso: targeted N (1..12)\n"; return 2;
    }
    std::cout << (bad ? "  => ESITO: PROBLEMI TROVATI\n" : "  => ESITO: ok\n");
    return bad ? 1 : 0;
}
