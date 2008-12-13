/*
 * Copyright (c) 2006-2008 The LOST Project. All rights reserved.
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

#ifndef _TREE_H_
#define _TREE_H_

//#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>

/**
 * Repraesentiert einen Knoten und bestimmt seine Position im Baum.
 *
 * Diese Struktur muss in jedem Typ, der in Baeumen verwendet werden soll,
 * als Teil enthalten sein. Durch den Offset, den das tree_item in dem
 * jeweiligen Typ hat, kann ein Pointer auf den Anfang des Objekts berechnet
 * werden.
 */
struct tree_item {
    struct tree_item* parent;
    struct tree_item* left;
    struct tree_item* right;
    int balance;
};

/**
 * Repraesentiert den Baum und erlaubt den Zugriff auf seine Elemente.
 *
 * Ein Baum besteht aus Objekten eines einheitlichen Datentyps. Die Objekte
 * enthalten jeweils mindestens ein tree_item, das Vater und Kinder sowie den
 * AVL-Balancewert ihres Knotens beschreibt und einen Schluessel, nach dem der
 * Baum sortiert wird. Der Schluessel muss vom Typ uint64_t sein.
 */
typedef struct {
    struct tree_item*   root;
    size_t              tree_item_offset;
    size_t              sort_key_offset;
} tree_t;

/**
 * Erzeugt einen neuen AVL-Baum
 *
 * @param type Datentyp der Objekte im Baum
 * @param tree_item Name des tree_item-Felds in der Struktur der Objekte
 * @param sort_key Name des Schluessels in der Struktur der Objekte
 */
#define tree_create(type, tree_item, sort_key) \
    tree_do_create(offsetof(type, tree_item), offsetof(type, sort_key))

/**
 * Erzeugt einen neuen AVL-Baum. Nicht direkt verwenden, tree_create ist das
 * Mittel der Wahl.
 */
tree_t* tree_do_create(size_t tree_item_offset, size_t sort_key_offset);

/**
 * Gibt einen AVL-Baum frei. Zu beachten ist, dass keiner seiner Knoten
 * freigegeben wird, da ein Knoten immer noch ueber eine andere Datenstruktur
 * erreichbar sein koennte.
 */
void tree_destroy(tree_t* tree);

/**
 * Sucht nach dem Objekt mit einem gegebenen Schluessel in einem AVL-Baum
 *
 * @return Objekt mit dem gesuchten Schluessel oder NULL, wenn kein Objekt mit
 * dem gesuchten Schluessel im Baum enthalten ist.
 */
void* tree_search(tree_t* tree, uint64_t key);

/**
 * Fuegt ein neues Objekt in den AVL-Baum ein.
 *
 * @param node Einzufuegendes Objekt. Muss den in tree_create angegebenen
 * Datentyp haben, ansonsten ist das Ergebnis undefiniert.
 */
tree_t* tree_insert(tree_t* tree, void* node);

/**
 * Entfernt ein Objekt aus dem AVL-Baum. Das uebergebene Objekt muss im Baum
 * enthalten sein.
 *
 * @return Das zu entfernende Objekt
 */
void* tree_remove(tree_t* tree, void* node);

/**
 * Sucht zu einem gegebenen Objekt den Vorgaenger.
 *
 * Der Vorgaenger ist das Objekt mit dem naechstkleineren Schluessel. Der
 * Vorgaenger von NULL ist das Objekt mit dem groessten Schluessel.
 *
 * @return Das Vorgaengerobjekt oder NULL, wenn das uebergebene Objekt das
 * Objekt mit dem kleinsten Schluessel war.
 */
void* tree_prev(tree_t* tree, void* node);

/**
 * Sucht zu einem gegebenen Objekt den Nachfolger
 *
 * Der Nachfolger ist das Objekt mit dem naechstgroesseren Schluessel. Der
 * Nachfolger von NULL ist das Objekt mit dem kleinsten Schluessel.
 *
 * @return Das Nachfolgerobjekt oder NULL, wenn das uebergebene Objekt das
 * Objekt mit dem groessten Schluessel war.
 */
void* tree_next(tree_t* tree, void* node);

#endif
