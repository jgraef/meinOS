/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Kevin Wolf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *     This product includes software developed by the LOST Project
 *     and its contributors.
 * 4. Neither the name of the LOST Project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <tree.h>
#include <malloc.h>

#ifdef DEBUG_TREE
  #include <stdio.h>

  void tree_dump(tree_t* tree);
#endif

/**
 * Gibt das Objekt zu einem tree_item zurueck
 */
static inline void* to_node(tree_t* tree, struct tree_item* item) {
    return ((char*) item - tree->tree_item_offset);
}

/**
 * Gibt das tree_item zu einem Objekt zurueck
 */
static inline struct tree_item* to_tree_item(tree_t* tree, void* node) {
    return (struct tree_item*) ((char*) node + tree->tree_item_offset);
}

/**
 * Gibt den Schluessel eines Objekts zurueck
 */
static inline uint64_t get_key(tree_t* tree, struct tree_item* item) {
    return *(uint64_t*)((char*) to_node(tree, item) + tree->sort_key_offset);
}


/**
 * Erzeugt einen neuen AVL-Baum. Nicht direkt verwenden, tree_create ist das
 * Mittel der Wahl.
 */
tree_t* tree_do_create(size_t tree_item_offset, size_t sort_key_offset)
{
    tree_t* tree = calloc(1, sizeof(*tree));

    tree->tree_item_offset = tree_item_offset;
    tree->sort_key_offset = sort_key_offset;

    return tree;
}

/**
 * Gibt einen AVL-Baum frei. Zu beachten ist, dass keiner seiner Knoten
 * freigegeben wird, da ein Knoten immer noch ueber eine andere Datenstruktur
 * erreichbar sein koennte.
 */
void tree_destroy(tree_t* tree)
{
    free(tree);
}

/**
 * Sucht das Objekt mit dem gegebenen Schluessel im Baum.
 *
 * @param tree Baum, in dem das Objekt gesucht werden soll
 * @param key Schluessel, nach dem gesucht wird
 * @param node Enthaelt das tree_item des gesuchten Objekts, wenn der
 * Schluessel gefunden wird. Wenn nicht, enthaelt es das tree_item des Blatts,
 * an das ein neuer Knoten mit dem gesuchten Schluessel angefuegt werden
 * muesste.
 *
 * @return 0 wenn das Objekt nicht gefunden wurde, nicht 0 sonst
 */
static int do_search(tree_t* tree, uint64_t key, struct tree_item** node)
{
    struct tree_item* res = NULL;
    struct tree_item* current = tree->root;
    uint64_t current_key = key - 1;

    while (current != NULL) {
        res = current;
        current_key = get_key(tree, current);
        if (key == current_key) {
            break;
        } else if (key < current_key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    *node = res;
    return (key == current_key);
}

/**
 * Sucht nach dem Objekt mit einem gegebenen Schluessel in einem AVL-Baum
 *
 * @return Objekt mit dem gesuchten Schluessel oder NULL, wenn kein Objekt mit
 * dem gesuchten Schluessel im Baum enthalten ist.
 */
void* tree_search(tree_t* tree, uint64_t key)
{
    struct tree_item* ret;

    if (do_search(tree, key, &ret)) {
        return to_node(tree, ret);
    } else {
        return NULL;
    }
}

/**
 * Aendert die Balance eines Knotens und passt die Balance aller Knoten auf dem
 * Weg zur Wurzel des Baums entsprechend an.
 *
 * @param item tree_item, dessen Balance geaendert werden soll
 * @param value Wert, um den die Balance geaendert werden soll
 */
static inline void adjust_balance(struct tree_item* item, int value)
{
    int stop = 0;

    while (item->parent && !stop) {
        if (item->parent->left == item) {
            // Wenn an einem rechtslastigen Knoten links etwas geaendert wird,
            // aendert sich zwar seine eigenen Balance, aber nicht mehr die des
            // Vaterknotens.
            if (item->parent->balance > 0) {
                stop = 1;
            }
            item->parent->balance -= value;
            if (item->parent->balance > 0) {
                stop = 1;
            }
        } else {
            // Dasselbe gilt fuer die rechte Seite eines linkslastigen Knotens.
            if (item->parent->balance < 0) {
                stop = 1;
            }
            item->parent->balance += value;
            if (item->parent->balance < 0) {
                stop = 1;
            }
        }
        item = item->parent;
    }
}

/**
 * Fuehrt eine Linksrotation durch. Balanceaenderungen werden nicht
 * vorgenommen.
 */
static void rotate_left(struct tree_item** root)
{
    struct tree_item* item = *root;
    struct tree_item* tmp = item->right;

    tmp->parent = item->parent;
    item->parent = tmp;
    if (tmp->left) {
        tmp->left->parent = item;
    }

    item->right = tmp->left;
    tmp->left = item;
    *root = tmp;
}

/**
 * Fuehrt eine Rechsrotation durch. Balanceaenderungen werden nicht
 * vorgenommen.
 */
static void rotate_right(struct tree_item** root)
{
    struct tree_item* item = *root;
    struct tree_item* tmp = item->left;

    tmp->parent = item->parent;
    item->parent = tmp;
    if (tmp->right) {
        tmp->right->parent = item;
    }

    item->left = tmp->right;
    tmp->right = item;
    *root = tmp;
}


/**
 * Gibt einen Zeiger auf den Zeiger des Vaterknotens auf den aktuellen Knoten
 * zurueck. Dieser Zeiger kann genutzt werden, um den gegebenen Knoten durch
 * einen anderen Knoten zu ersetzen.
 *
 * Im Fall, dass der gegebene Knoten die Wurzel ist, wird &tree->root
 * zurueckgegeben, so dass in diesem Fall eine neue Wurzel gesetzt wird.
 */
static struct tree_item** link_from_parent(tree_t* tree,
    struct tree_item* item)
{
    if (item->parent == NULL) {
        return &tree->root;
    } else if (item->parent->left == item) {
        return &item->parent->left;
    } else {
        return &item->parent->right;
    }
}

/**
 * Fuehrt eine AVL-Rebalancierung durch, nachdem der Baum veraendert wurde.
 */
static void rebalance(tree_t* tree, struct tree_item* item)
{
    struct tree_item** pitem;
    struct tree_item* tmp;

    while (item) {

        pitem = link_from_parent(tree, item);

        if (item->balance >= 2) {

            // Rechtslastiger Baum
            tmp = item->right;

            if (item->right->balance > 0) {
                // Einfache Rotation
                rotate_left(pitem);
                item->balance = 0;
                tmp->balance = 0;
                adjust_balance(*pitem, -1);
            } else if (item->right->balance == 0) {
                // Einfache Rotation
                rotate_left(pitem);
                item->balance = 1;
                tmp->balance = -1;
            } else {
                // Doppelrotation
                rotate_right(&item->right);
                rotate_left(pitem);
                item->balance = ((*pitem)->balance ==  1 ? -1 : 0);
                tmp->balance  = ((*pitem)->balance == -1 ?  1 : 0);
                (*pitem)->balance = 0;
                adjust_balance(*pitem, -1);
            }


        } else if (item->balance <= -2) {

            // Linkslastiger Baum
            tmp = item->left;

            if (item->left->balance < 0) {
                // Einfache Rotation
                rotate_right(pitem);
                item->balance = 0;
                tmp->balance = 0;
                adjust_balance(*pitem, -1);
            } else if (item->left->balance == 0) {
                // Einfache Rotation
                rotate_right(pitem);
                item->balance = -1;
                tmp->balance = 1;
            } else {
                // Doppelrotation
                rotate_left(&item->left);
                rotate_right(pitem);
                item->balance = ((*pitem)->balance == -1 ?  1 : 0);
                tmp->balance  = ((*pitem)->balance ==  1 ? -1 : 0);
                (*pitem)->balance = 0;
                adjust_balance(*pitem, -1);
            }
        }

        // Weiter Richtung Wurzel gehen
        item = item->parent;

    }
}

/**
 * Fuegt ein neues Objekt in den AVL-Baum ein.
 *
 * @param node Einzufuegendes Objekt. Muss den in tree_create angegebenen
 * Datentyp haben, ansonsten ist das Ergebnis undefiniert.
 */
tree_t* tree_insert(tree_t* tree, void* node)
{
    struct tree_item* new = to_tree_item(tree, node);
    struct tree_item* parent;
    uint64_t new_key = get_key(tree, new);

    new->left = NULL;
    new->right = NULL;
    new->balance = 0;

    // Wenn es noch keine Wurzel gibt, ist die Sache trivial
    if (tree->root == NULL) {
        new->parent = NULL;
        tree->root = new;
        return tree;
    }

    // Ansonsten erstmal einen nahegelegenen Knoten finden, an dem man den
    // neuen Knoten einhaengen kann. Wenn ein Knoten mit derselben ID schon
    // drin ist, machen wir gar nichts.
    if (!do_search(tree, new_key, &parent)) {
        new->parent = parent;

        if (new_key < get_key(tree, parent)) {
            parent->left = new;
        } else {
            parent->right = new;
        }
        adjust_balance(new, 1);
        rebalance(tree, new);
    }

    return tree;
}

/**
 * Entfernt ein Objekt aus dem AVL-Baum. Das uebergebene Objekt muss im Baum
 * enthalten sein.
 *
 * @return Das zu entfernende Objekt
 */
void* tree_remove(tree_t* tree, void* node)
{
    struct tree_item* item = to_tree_item(tree, node);
    struct tree_item** pitem = link_from_parent(tree, item);

    // Wenn es ein Blatt war, einfach rausnehmen

    if (!item->left && !item->right) {
        adjust_balance(item, -1);
        *pitem = NULL;
        rebalance(tree, item->parent);
        return node;
    }

    // Ansonsten muessen wir die Kinder irgendwo anders aufhaengen. Wenn nur
    // eine Seite besetzt ist, uebernimmt einfach das Kind dort den Platz.

    if (item->left && !item->right) {
        adjust_balance(item, -1);
        *pitem = item->left;
        item->left->parent = item->parent;
        rebalance(tree, item->parent);
        return node;
    }

    if (!item->left && item->right) {
        adjust_balance(item, -1);
        *pitem = item->right;
        item->right->parent = item->parent;
        rebalance(tree, item->parent);
        return node;
    }

    // Bleibt schliesslich noch der komplizierteste Fall, naemlich, dass beide
    // Seiten besetzt sind. Dann nehmen wir einen vom Index her nahegelegenen
    // Knoten als Ersatz fuer den geloeschten. Ein guter Kandidat dafuer ist
    // der am weitesten rechts gelegene Knoten des linken Teilbaums.

    struct tree_item* subst = to_tree_item(tree, node);
    struct tree_item** psubst;

    subst = item->left;
    while (subst->right) {
        subst = subst->right;
    }

    psubst = link_from_parent(tree, subst);

    // Wir sind ganz nach rechts gegangen, aber moeglicherweise ist noch ein
    // linker Teilbaum uebrig. Dieser wird dann vom bisherigen Elternknoten
    // des Ersatzes geerbt.

    struct tree_item* old_left = subst->left;
    struct tree_item* old_parent = subst->parent;

    subst->parent   = item->parent;
    subst->right    = item->right;
    subst->left     = item->left;
    subst->balance  = item->balance;

    if (subst->left == subst) {
        subst->left = NULL;
    }

    if (subst->left) {
        subst->left->parent = subst;
    }
    if (subst->right) {
        subst->right->parent = subst;
    }

    *psubst = old_left;
    *pitem = subst;

    if (old_parent == item) {
        subst->balance++;
        subst->left = old_left;
        if (subst->balance <= 0) {
            adjust_balance(subst, -1);
        }
        rebalance(tree, subst);
    } else {
        if (old_left) {
            old_left->parent = old_parent;
        }

        old_parent->balance--;
        if (old_parent->balance >= 0) {
            adjust_balance(old_parent, -1);
        }
        rebalance(tree, old_parent);
    }

    return node;
}

/**
 * Sucht zu einem gegebenen Objekt den Vorgaenger.
 *
 * Der Vorgaenger ist das Objekt mit dem naechstkleineren Schluessel. Der
 * Vorgaenger von NULL ist das Objekt mit dem groessten Schluessel.
 *
 * @return Das Vorgaengerobjekt oder NULL, wenn das uebergebene Objekt das
 * Objekt mit dem kleinsten Schluessel war.
 */
void* tree_prev(tree_t* tree, void* node)
{
    struct tree_item* current = to_tree_item(tree, node);
    struct tree_item* old = current;

    if (node == NULL) {
        current = tree->root;
    } else {

        // Irgendwo mitten im Baum ist es einfach, den Vorgaenger zu finden:
        // Einmal nach links und dann so weit wie moeglich nach rechts.
        //
        // Wenn der Knoten keinen linken Kindknoten hat, muessen wir ein Stueck
        // Richtung Wurzel zurueckgehen, und zwar so lange bis wir von der
        // rechten Seite an einen Knoten kommen.

        if (current->left == NULL) {
            do {
                old = current;
                current = current->parent;
            } while (current && (current->left == old));

            return current ? to_node(tree, current) : NULL;
        }

        current = current->left;
    }

    while (current->right) {
        current = current->right;
    }

    return to_node(tree, current);
}

/**
 * Sucht zu einem gegebenen Objekt den Nachfolger
 *
 * Der Nachfolger ist das Objekt mit dem naechstgroesseren Schluessel. Der
 * Nachfolger von NULL ist das Objekt mit dem kleinsten Schluessel.
 *
 * @return Das Nachfolgerobjekt oder NULL, wenn das uebergebene Objekt das
 * Objekt mit dem groessten Schluessel war.
 */
void* tree_next(tree_t* tree, void* node)
{
    struct tree_item* current = to_tree_item(tree, node);
    struct tree_item* old = current;

    if (node == NULL) {
        current = tree->root;
    } else {
        // Das ist im Prinzip ein tree_prev mit links und rechts vertauscht

        if (current->right == NULL) {
            do {
                old = current;
                current = current->parent;
            } while (current && (current->right == old));

            return current ? to_node(tree, current) : NULL;
        }

        current = current->right;
    }

    while (current->left) {
        current = current->left;
    }

    return to_node(tree, current);
}


#ifdef DEBUG_TREE
static void do_check(tree_t* tree, struct tree_item* item, struct tree_item* parent)
{
    if (item->parent != parent) {
        printf("Oops, elternlink  stimmt nicht: %lld\n", (unsigned long long)
            get_key(tree, item));
    }

    if (item->left) {
        do_check(tree, item->left, item);
    }
    if (item->right) {
        do_check(tree, item->right, item);
    }
}

void tree_check(tree_t* tree)
{
    if (tree->root) {
        do_check(tree, tree->root, NULL);
    }
}

/**
 * Gibt ein Element und alle Kindelemente aus
 */
static void do_dump(tree_t* tree, struct tree_item* current)
{
    if (!current) {
        printf("-");
    } else if (!current->left && !current->right) {
        printf("%ld", get_key(tree, current));
    } else {
        printf("[%d] %ld (", current->balance, get_key(tree, current));
        do_dump(tree, current->left),
        printf(" ");
        do_dump(tree, current->right);
        printf(")");
    }
}

/**
 * Gibt einen Dump eines Baums auf stdout aus
 */
void tree_dump(tree_t* tree)
{
    do_dump(tree, tree->root);
    puts("");
}
#endif
