#include <iostream>
#include <queue>
#include <set>
#include <random>
#include <vector>
#include <algorithm>
#include <string>
#include <climits>
#include <cmath>
#include <sstream>
#define private public
#define protected public
#include "RBTree.h"
#undef private
#undef protected

typedef Node<int> N;

// ritorna "" se l'albero e' valido, altrimenti il motivo
struct Checker {
    std::set<int> seen;
    std::string err;
    int maxdepth = 0;

    // ritorna l'altezza nera (conta i nodi neri lungo il cammino, nil escluso), -1 se errore
    int walk(N* n, N* par, long lo, long hi, int depth) {
        if (!n) return 0;
        if (depth > maxdepth) maxdepth = depth;
        int v = n->get_value();
        if (n->get_parent() != par) { err = "parent errato su " + std::to_string(v); return -1; }
        if (v <= lo || v >= hi)     { err = "ordine BST violato su " + std::to_string(v); return -1; }
        if (!seen.insert(v).second) { err = "valore duplicato " + std::to_string(v); return -1; }
        COLORS c = n->get_color();
        if (c != RED && c != BLACK) { err = "colore NONE/invalido su " + std::to_string(v); return -1; }
        N* l = n->get_left(); N* r = n->get_right();
        if (c == RED) {
            if ((l && l->get_color() == RED) || (r && r->get_color() == RED)) {
                err = "rosso con figlio rosso su " + std::to_string(v); return -1;
            }
        }
        int lh = walk(l, n, lo, v, depth + 1); if (lh < 0) return -1;
        int rh = walk(r, n, v, hi, depth + 1); if (rh < 0) return -1;
        if (lh != rh) { err = "altezza nera diversa sotto " + std::to_string(v) +
                              " (sx=" + std::to_string(lh) + ", dx=" + std::to_string(rh) + ")"; return -1; }
        return lh + (c == BLACK ? 1 : 0);
    }

    std::string check(RBTree<int>& t, const std::set<int>& ref) {
        seen.clear(); err.clear(); maxdepth = 0;
        if (t.root) {
            if (t.root->get_parent() != NULL) return "la radice ha un parent";
            if (t.root->get_color() != BLACK) return "radice non nera";
        }
        if (walk(t.root, NULL, LONG_MIN, LONG_MAX, 1) < 0) return err;
        if (seen != ref) return "insieme dei valori diverso da std::set (size albero=" +
                                std::to_string(seen.size()) + ", atteso=" + std::to_string(ref.size()) + ")";
        // altezza massima ammessa per un RB tree: 2*log2(n+1)
        if (!ref.empty()) {
            double lim = 2.0 * std::log2((double)ref.size() + 1.0);
            if (maxdepth > lim + 1e-9) return "altezza " + std::to_string(maxdepth) + " > limite " + std::to_string(lim);
        }
        return "";
    }
};

static int failures = 0;
static void report(const std::string& name, const std::string& e, const std::string& ctx) {
    failures++;
    std::cout << "FALLITO [" << name << "]: " << e << "\n   contesto: " << ctx << "\n";
}

int main(int argc, char** argv) {
    (void)argv;
    Checker ck;

    // 1. casuale su dominio piccolo, con rimozioni di valori assenti e inserimenti duplicati
    {
        std::mt19937 rng(12345);
        for (int run = 0; run < 3000 && failures == 0; run++) {
            RBTree<int> t; std::set<int> ref; std::string log;
            for (int k = 0; k < 120; k++) {
                int v = rng() % 40;
                if (rng() % 2) { N* r = t.insert(v); bool fresh = ref.insert(v).second;
                                 if ((r != NULL) != fresh) { report("random-small", "valore di ritorno di insert errato", "i" + std::to_string(v)); break; }
                                 log += "i" + std::to_string(v) + " "; }
                else           { t.remove(v); ref.erase(v); log += "r" + std::to_string(v) + " "; }
                std::string e = ck.check(t, ref);
                if (!e.empty()) { report("random-small", e, log); break; }
            }
        }
        std::cout << "[1] random dominio 0..39, 3000 sequenze x 120 op: " << (failures ? "FALLITO" : "ok") << "\n";
    }

    // 2. casuale su dominio grande
    if (!failures) {
        std::mt19937 rng(777);
        for (int run = 0; run < 150 && failures == 0; run++) {
            RBTree<int> t; std::set<int> ref; std::string log;
            for (int k = 0; k < 800; k++) {
                int v = rng() % 1000;
                // due terzi inserimenti all'inizio, poi piu' rimozioni
                bool ins = (k < 400) ? (rng() % 3 != 0) : (rng() % 3 == 0);
                if (ins) { t.insert(v); ref.insert(v); } else { t.remove(v); ref.erase(v); }
                log = std::string(ins ? "i" : "r") + std::to_string(v);
                std::string e = ck.check(t, ref);
                if (!e.empty()) { report("random-large", e, "ultima op " + log + " run " + std::to_string(run)); break; }
            }
        }
        std::cout << "[2] random dominio 0..999, 150 sequenze x 800 op: " << (failures ? "FALLITO" : "ok") << "\n";
    }

    // 3. ordinati e permutazioni: insert tutto, poi remove tutto in vari ordini
    if (!failures) {
        const int N_ = 1500;
        std::mt19937 rng(99);
        std::vector<std::string> modes = {"asc/asc", "asc/desc", "desc/asc", "desc/desc", "rand/rand", "asc/rand", "rand/asc", "asc/centro-fuori"};
        for (auto& mode : modes) {
            RBTree<int> t; std::set<int> ref;
            std::vector<int> ins(N_), del(N_);
            for (int i = 0; i < N_; i++) ins[i] = del[i] = i;
            std::string a = mode.substr(0, mode.find('/')), b = mode.substr(mode.find('/') + 1);
            if (a == "desc") std::reverse(ins.begin(), ins.end());
            if (a == "rand") std::shuffle(ins.begin(), ins.end(), rng);
            if (b == "desc") std::reverse(del.begin(), del.end());
            if (b == "rand") std::shuffle(del.begin(), del.end(), rng);
            if (b == "centro-fuori") { int m = N_ / 2; del.clear(); for (int d = 0; d < N_; d++) { int v = (d % 2 == 0) ? m + d / 2 : m - 1 - d / 2; if (v >= 0 && v < N_) del.push_back(v); } }
            for (int v : ins) { t.insert(v); ref.insert(v); if (v % 50 == 0 || v == N_ - 1) { auto e = ck.check(t, ref); if (!e.empty()) { report("ordinati " + mode, e, "dopo insert " + std::to_string(v)); goto next_mode; } } }
            { auto e = ck.check(t, ref); if (!e.empty()) { report("ordinati " + mode, e, "dopo tutti gli insert"); goto next_mode; } }
            for (size_t i = 0; i < del.size(); i++) { int v = del[i]; t.remove(v); ref.erase(v);
                auto e = ck.check(t, ref); if (!e.empty()) { report("ordinati " + mode, e, "dopo remove " + std::to_string(v) + " (passo " + std::to_string(i) + ")"); goto next_mode; } }
            if (t.root != NULL) report("ordinati " + mode, "radice non nulla dopo aver rimosso tutto", "");
            next_mode:;
        }
        std::cout << "[3] ordini asc/desc/casuale/centro-fuori, N=1500, controllo a ogni rimozione: " << (failures ? "FALLITO" : "ok") << "\n";
    }

    // 4. rimozione ripetuta della radice
    if (!failures) {
        std::mt19937 rng(5);
        for (int run = 0; run < 300 && failures == 0; run++) {
            RBTree<int> t; std::set<int> ref;
            int n = 1 + rng() % 60;
            for (int i = 0; i < n; i++) { int v = rng() % 200; t.insert(v); ref.insert(v); }
            while (t.root) {
                int v = t.root->get_value();
                t.remove(v); ref.erase(v);
                auto e = ck.check(t, ref);
                if (!e.empty()) { report("rimuovi-radice", e, "valore radice rimosso " + std::to_string(v)); break; }
            }
        }
        std::cout << "[4] rimozione ripetuta della radice fino a svuotare: " << (failures ? "FALLITO" : "ok") << "\n";
    }

    // 5. esaustivo: ogni permutazione di inserimento x ogni permutazione di rimozione (n=6)
    if (!failures && argc > 1) {
        const int n = 6;
        std::vector<int> pi(n), pd(n);
        long combos = 0;
        for (int i = 0; i < n; i++) pi[i] = i;
        do {
            for (int i = 0; i < n; i++) pd[i] = i;
            do {
                RBTree<int> t; std::set<int> ref; std::string log;
                for (int v : pi) { t.insert(v); ref.insert(v); log += "i" + std::to_string(v) + " "; }
                auto e = ck.check(t, ref);
                if (e.empty()) for (int v : pd) { t.remove(v); ref.erase(v); log += "r" + std::to_string(v) + " "; e = ck.check(t, ref); if (!e.empty()) break; }
                combos++;
                if (!e.empty()) { report("esaustivo n=6", e, log); goto done5; }
            } while (std::next_permutation(pd.begin(), pd.end()));
        } while (std::next_permutation(pi.begin(), pi.end()));
        done5:
        std::cout << "[5] esaustivo n=6: " << combos << " combinazioni (insert x remove): " << (failures ? "FALLITO" : "ok") << "\n";
    }

    std::cout << (failures ? "\nRISULTATO: FALLITO\n" : "\nRISULTATO: tutti i test superati\n");
    return failures ? 1 : 0;
}
