# rbtree-cpp

Piccola libreria C++ a template (solo header) con:

- `Node<T>` — nodo con puntatori a padre, figlio sinistro e destro e colore;
- `Tree<T>` — albero binario di ricerca (BST);
- `RBTree<T>` — albero rosso-nero che **eredita da `Tree<T>`**: riusa inserimento e rimozione del BST e poi ripristina le proprietà rosso-nere (ricolorazioni e rotazioni).

Progetto didattico: l'obiettivo è avere una struttura BST compatta su cui costruire il bilanciamento.

## Uso

La libreria è solo header: basta aggiungere `include/` al percorso di inclusione.

```cpp
#include "RBTree.h"

RBTree<int> t;
t.insert(5);
t.insert(3);
t.remove(5);
if (t.lookup(3)) { /* trovato */ }
t.print();             // stampa i nodi a livelli, con colore e figli

RBTree<int> copia = t; // copia profonda (valori e colori)
```

```bash
g++ -std=c++17 -Iinclude esempio.cpp -o esempio
```

## API

| Metodo | Descrizione |
|---|---|
| `Node<T>* insert(T)` | Inserisce il valore. Restituisce il nodo creato, oppure `NULL` se il valore era già presente (i duplicati sono ignorati). |
| `Node<T>* remove(T)` | Rimuove il valore. Se è assente non fa nulla. Il valore restituito è il nodo che ha preso il posto di quello rimosso (usato internamente per il ripristino delle proprietà) e può essere `NULL`: normalmente si ignora. |
| `Node<T>* lookup(T)` | Restituisce il nodo con quel valore, o `NULL`. |
| `void print()` | Stampa ogni nodo con colore (`B`/`R`) e figli, a livelli. |
| Costruttore di copia, `operator=` | Copia profonda; l'assegnazione libera il contenuto precedente e gestisce l'autoassegnazione. |
| Distruttore | Virtuale: si può distruggere un `RBTree` tramite `Tree<T>*`. |

`insert` e `remove` sono virtuali: chiamandoli tramite un riferimento o puntatore a `Tree<T>` su un `RBTree` vengono comunque eseguiti i riequilibri rosso-neri.

## Requisiti sul tipo `T`

- operatori `<`, `>`, `==`;
- **costruttore di default** (verificato: un tipo senza costruttore di default non compila);
- `operator<<` per `std::ostream` solo se si usa `print()`.

Funziona con `int`, `double`, `std::string` (testati).

## Complessità

`insert`, `remove` e `lookup` sono O(log n): l'altezza resta entro 2·log₂(n+1). Il distruttore e `operator=` svuotano l'albero con una `remove` per nodo.

## Test

```bash
make test         # test casuali, ordinati e mirati
make sanitize     # gli stessi test con AddressSanitizer + UBSan (rileva perdite di memoria)
make exhaustive   # tutte le permutazioni di inserimento x rimozione con 6 elementi (518.400 casi)
make demo         # esempio d'uso
make known-issues # mostra la limitazione nota descritta sotto
```

I test (`tests/`) verificano a ogni operazione le proprietà rosso-nere (radice nera, nessun rosso con figlio rosso, stessa altezza nera su ogni cammino), i puntatori `parent`, l'ordinamento BST e il contenuto confrontato con `std::set`. I file di test richiedono C++11 o successivo; la libreria stessa è stata compilata anche in C++98.

Compilatori provati: g++ 13.3 e clang 18 (libstdc++), da C++98 a C++23. libc++ e MSVC non sono stati provati.

## Limitazioni note

- **Slicing:** `Tree<T> s = un_rbtree;` copia i valori ma non i colori (il costruttore di copia di `Tree` non può chiamare la `clone` di `RBTree`, perché in un costruttore le chiamate virtuali non arrivano alla classe derivata). Si veda `make known-issues`. Si evita copiando `RBTree` in `RBTree`.
- I valori duplicati vengono ignorati.
- I getter/setter di `Node` non sono `const` (scelta attuale del progetto).
- Compilando con `-Wall -Wextra` il compilatore segnala avvisi innocui: `-Woverloaded-virtual` su `Tree::operator=` e `-Wmaybe-uninitialized` in `Tree::print`.

## Struttura

```
include/    Node.h/.tpp, Tree.h/.tpp, RBTree.h/.tpp
tests/      stress.cpp (casuali, ordinati, esaustivo), targeted.cpp (casi mirati T1-T12)
examples/   demo.cpp
```

## Licenza

MIT — vedi `LICENSE`.
