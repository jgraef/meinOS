/*
 * Copyright (c) 2008 Antoine Kaufmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree.h>

#include "cdi/lists.h"
#include "cdi/cache.h"

#define INVBLKNUM (~0L)
#define READBUF_SZ 4
#define HINTCNT 8

/**
 * Block im Cache
 */
struct block {
    /** CDI-Interface-Block */
    struct cdi_cache_block  cdi;

    /** Anzahl der Eintraege, die diesen Block beinhalten */
    uint16_t                ref_count;

    /** Wird auf 1 gesetzt, wenn die Daten veraendert wurden */
    uint16_t                dirty;

    /** Zeiger auf das naechste Element in der nach der letzten Benutzung
        sortierten Liste. */
    struct block*           lru_next;

    /** Zeiger auf das naechste Element in der nach der letzten Benutzung
        sortierten Liste. */
    struct block*           lru_prev;

    struct tree_item        tree_item;
};

/**
 * Cache-Handle mit OS-spezifischen Daten
 */
struct cache {
    /** Eigentliches Cache-Handle */
    struct cdi_cache    cache;

    /** Groesse der Privaten Daten pro Block */
    size_t              private_len;

    /** Anzahl der Blocks */
    size_t              block_count;

    /** Anzahl der benutzten Blocks */
    size_t              blocks_used;


    /** Callback zum Lesen eines Blocks */
    cdi_cache_read_block_t* read_block;

    /** Callback zum Schreiben eines Blocks */
    cdi_cache_write_block_t* write_block;

    /** Letzter Parameter fuer die Callbacks */
    void*               prv_data;


    /** Baum mit den Blocks */
    tree_t*             tree;


    /** Der am laengsten nicht mehr benutzte Block */
    struct block*       lru_first;

    /** Der zuletzt benutzte Block */
    struct block*       lru_last;



    /** Lesepuffer */
    uint8_t*            read_buffer;

    /** Erster Block im Lesepuffer oder INVBLKNUM wenn keiner */
    uint64_t            read_buffer_block;

    /** Anzahl Blocks die im Puffer sind */
    size_t              read_buffer_cnt;


    /** Hints */
    struct block*        hints[HINTCNT];

    /** Letzter abgefragter Hint */
    uint8_t             prev_hint;
};


/**
 * Cache erstellen
 *
 * @param block_size    Groesse der Blocks die der Cache verwalten soll
 * @param blkpriv_len   Groesse der privaten Daten die fuer jeden Block
 *                      alloziert werden und danach vom aurfrufer frei benutzt
 *                      werden duerfen
 * @param read_block    Funktionspointer auf eine Funktion zum einlesen eines
 *                      Blocks.
 * @param write_block   Funktionspointer auf eine Funktion zum schreiben einses
 *                      Blocks.
 * @param prv_data      Wird den Callbacks als letzter Parameter uebergeben
 *
 * @return Pointer auf das Cache-Handle
 */
struct cdi_cache* cdi_cache_create(size_t block_size, size_t blkpriv_len,
    cdi_cache_read_block_t* read_block,
    cdi_cache_write_block_t* write_block,
    void* prv_data)
{
    struct cache* cache = malloc(sizeof(*cache));
    int i;

    cache->read_block = read_block;
    cache->write_block = write_block;
    cache->prv_data = prv_data;

    // FIXME
    cache->cache.block_size = block_size;
    cache->block_count = 256;
    cache->blocks_used = 0;
    cache->private_len = blkpriv_len;

    cache->read_buffer = malloc(cache->cache.block_size * READBUF_SZ);
    cache->read_buffer_block = INVBLKNUM;
    cache->read_buffer_cnt = 0;

    cache->lru_first = NULL;
    cache->lru_last = NULL;

    cache->tree = tree_create(struct block, tree_item, cdi.number);

    // Hints initialisieren
    cache->prev_hint = 0;
    for (i = 0; i < HINTCNT; i++) {
        cache->hints[i] = NULL;
    }

    return (struct cdi_cache*) cache;
}

/**
 * Cache zerstoeren
 */
void cdi_cache_destroy(struct cdi_cache* cache)
{
    struct cache* c = (struct cache*) cache;
    struct block* b = NULL;

    if (!cdi_cache_sync(cache)) {
        puts("cdi_cache: Sync fehlgeschlagen vor dem Zerstoeren!");
    }

    // Einzelne Blocks freigeben
    while ((b = tree_next(c->tree, NULL))) {

        tree_remove(c->tree, b);
        if (b->ref_count) {
            printf("cdi_cache: Beim Zerstoeren des Caches wurde ein Block "
                "gefunden, der einen Referenzzaehler != 0 hat (%lld  %d)\n",
                (unsigned long long) b->cdi.number, b->ref_count);
        }

        if (b->dirty) {
            printf("cdi_cache: Beim Zerstoeren des Caches wurde ein Block "
                "gefunden, der als veraendert markiert ist (%lld)\n",
                (unsigned long long) b->cdi.number);
        }

        free(b->cdi.data);
        free(b->cdi.private);
        free(b);
    }

    free(c);
}


/**
 * Einzelnen Block auf das Geraet schreiben. Das wird nur durchgefuehrt, wenn er
 * veraendert wurde.
 *
 * @param c Cache-Handle
 * @param b Block-Pointer
 *
 * @return 1 bei Erfolg, 0 im Fehlerfall
 */
static inline int cache_sync_block(struct cache* c, struct block* b)
{
    if (!b->dirty) {
        return 1;
    }

    // Lesepuffer aktualisieren, wenn noetig
    if ((c->read_buffer_block != INVBLKNUM) &&
        (c->read_buffer_block <= b->cdi.number) &&
        (b->cdi.number < c->read_buffer_block + c->read_buffer_cnt))
    {
        memcpy(c->read_buffer +
            (b->cdi.number - c->read_buffer_block) * c->cache.block_size,
            b->cdi.data, c->cache.block_size);
    }

    if (!c->write_block((struct cdi_cache*) c, b->cdi.number, 1, b->cdi.data,
        c->prv_data))
    {
        puts("cdi_cache: Schreiben beim Cache-Sync fehlgeschlagen.");
        return 0;
    }
    b->dirty = 0;

    return 1;
}


/**
 * Veraenderte Blocks aus dem Cache auf das Geraet schreiben
 *
 * @param cache Handle
 *
 * @return 1 bei Erfolg, 0 im Fehlerfall
 */
int cdi_cache_sync(struct cdi_cache* cache)
{
    struct cache* c = (struct cache*) cache;;
    struct block* b = NULL;

    while ((b = tree_prev(c->tree, b))) {
        cache_sync_block(c, b);
    }

    return 1;
}

/**
 * Block finden der ersetzt werden kann. Dieser wird dabei aus der LRU-Liste
 * geloescht.
 */
static struct block* find_replaceable(struct cache* c)
{
    struct block* b;

    // Unbenutzten Block finden
    b = c->lru_first;
    while (b && (b->ref_count)) {
        b = b->lru_prev;
    }

    if (b && b->dirty) {
        cdi_cache_sync((struct cdi_cache*) c);
    }
    return b;
}

/**
 * Hint suchen fuer eine Blocknummer
 *
 * @param block Blocknummer
 *
 * @return Index oder (size_t) -1 im Fehlerfall
 */
static inline struct block* get_hint(struct cache* cache, uint64_t block)
{
    int i;

    for (i = 0; i < HINTCNT; i++) {
        if (cache->hints[i] && (cache->hints[i]->cdi.number == block)) {
            return cache->hints[i];
        }
    }

    return NULL;
}

/**
 * Hint setzen
 *
 * @param block Blocknummer
 * @param index Index an dem der Block gefunden werden kann
 */
static inline void put_hint(struct cache* cache, struct block* block)
{
    cache->prev_hint++;
    cache->prev_hint %= HINTCNT;
    cache->hints[cache->prev_hint] = block;
}

/**
 * Block von der Platte laden.
 */
static int load_block(struct cache* c, struct block* b)
{
    size_t block_size = c->cache.block_size;
    uint64_t rbbnum = c->read_buffer_block;
    uint64_t block = b->cdi.number;

    // Wenn der zu lesende Block nicht im Puffer ist, wird er in den Puffer
    // eingelesen
    if ((rbbnum == INVBLKNUM) || (rbbnum > block) || (block >= rbbnum +
        c->read_buffer_cnt))
    {
        c->read_buffer_block = rbbnum = block;

        c->read_buffer_cnt = c->read_block(
            (struct cdi_cache*) c, block, READBUF_SZ,
            c->read_buffer, c->prv_data);

        if (c->read_buffer_cnt == 0) {
            puts("cdi_cache Panic: Einlesen der Daten fehlgeschlagen");
            return 0;
        }
    }
    memcpy(b->cdi.data, c->read_buffer + (block - rbbnum) * block_size,
        block_size);

    return 1;
}

/**
 * Cache-Block holen. Dabei wird intern ein Referenzzaehler erhoeht, sodass der
 * Block nicht freigegeben wird, solange er benutzt wird. Das heisst aber auch,
 * dass der Block nach der Benutzung wieder freigegeben werden muss, da sonst
 * die freien Blocks ausgehen.
 *
 * @param cache     Cache-Handle
 * @param blocknum  Blocknummer
 * @param noread    Wenn != 0 wird der Block nicht eingelesen falls er noch
 *                  nicht im Speicher ist. Kann benutzt werden, wenn der Block
 *                  vollstaendig ueberschrieben werden soll.
 *
 * @return Pointer auf das Block-Handle oder NULL im Fehlerfall
 */
struct cdi_cache_block* cdi_cache_block_get(struct cdi_cache* cache,
    uint64_t blocknum, int noread)
{
    struct cache* c = (struct cache*) cache;
    struct block* b = get_hint(c, blocknum);

    // Wenn das mit dem Hint nichts war muss jetzt der Baum durchsucht werden
    if (!b) {
        b = tree_search(c->tree, blocknum);
    }

    if (!b) {
        int in_tree = 0;
        if (c->blocks_used < c->block_count) {
            // Wir duerfen noch mehr neue Blocks in den Cache legen
            b = malloc(sizeof(*b));
            memset(b, 0, sizeof(*b));
            b->cdi.number = blocknum;
            b->cdi.data = malloc(c->cache.block_size);
            b->cdi.private = malloc(c->private_len);

            tree_insert(c->tree, b);
            c->blocks_used++;
        } else {
            // Wir muessen einen Block zum ersetzen suchen
            b = find_replaceable(c);
            if (!b) {
                return NULL;
            }
            in_tree = 1;
            tree_remove(c->tree, b);
            b->cdi.number = blocknum;
            tree_insert(c->tree, b);
        }

        // Block einlesen
        if (!noread && !load_block(c, b)) {
            puts("cdi_cache: Einlesen des Blocks fehlgeschlagen.");

            // Wenn der Knoten bis jetzt im Baum war, muss er rausgeworfen
            // werden
            if (in_tree) {
                tree_remove(c->tree, b);
            }
            c->blocks_used--;

            free(b->cdi.data);
            free(b);
            return NULL;
        }
    }
    put_hint(c, b);

    b->ref_count++;

    // Wenn wir eh schon den juengsten Block in der Liste erwischt haben,
    // muessen wir auch an der LRU-Liste nichts mehr aendern.
    if (b == c->lru_last) {
        return (struct cdi_cache_block*) b;
    }

    // LRU-Liste aktualisieren
    if (b->lru_next) {
        b->lru_next->lru_prev = b->lru_prev;
    }
    if (b->lru_prev) {
        b->lru_prev->lru_next = b->lru_next;
    }
    if (c->lru_first == b) {
        c->lru_first = b->lru_prev;
    }
    b->lru_next = c->lru_last;
    b->lru_prev = NULL;
    if (c->lru_last) {
        c->lru_last->lru_prev = b;
    } else {
        c->lru_first = b;
    }
    c->lru_last = b;

    return (struct cdi_cache_block*) b;
}

/**
 * Cache-Block freigeben
 *
 * @param cache Cache-Handle
 * @param block Block-Handle
 */
void cdi_cache_block_release(struct cdi_cache* cache,
    struct cdi_cache_block* block)
{
    struct block* b = (struct block*) block;

    b->ref_count--;
}

/**
 * Cache-Block als veraendert markieren
 *
 * @param cache Cache-Handle
 * @param block Block-Handle
 */
void cdi_cache_block_dirty(struct cdi_cache* cache,
    struct cdi_cache_block* block)
{
    struct block* b = (struct block*) block;
    b->dirty = 1;
}


