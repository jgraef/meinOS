/*
 * Copyright (c) 2008 Antoine Kaufmann
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _CDI_CACHE_H_
#define _CDI_CACHE_H_

#include <stdint.h>

struct cdi_cache {
    /** Groesse der Blocks, die der Cache verwaltet */
    size_t      block_size;

    /* OS-Spezifische Daten folgen... */
};

/** Block fuer direkten, schnellen Zugriff. */
struct cdi_cache_block {
    /** Blocknummer */
    uint64_t    number;

    /** Zeiger auf die Daten */
    void*       data;

    /** Pointer auf blkpriv_len Bytes, die vom Aufrufer benutzt werden koennen*/
    void*       private;
};

/** Cache-Bereiche fuer komfortablen, dafuer etwas langsameren, Zugriff */
struct cdi_cache_entry {
    struct cdi_cache* cache;

    /* OS-Spezifische Daten folgen... */
};

/** Typ fuer Cache-Callback zum einlesen eines Blocks. */
typedef int (cdi_cache_read_block_t)(struct cdi_cache* cache, uint64_t block,
    size_t count, void* dest, void* prv);

/** Typ fuer Cache-Callback zum schreiben eines Blocks. */
typedef int (cdi_cache_write_block_t)(struct cdi_cache* cache, uint64_t block,
    size_t count, const void* src, void* prv);


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
    void* prv_data);

/**
 * Cache zerstoeren
 */
void cdi_cache_destroy(struct cdi_cache* cache);

/**
 * Veraenderte Cache-Blocks auf die Platte schreiben
 *
 * @return 1 bei Erfolg, 0 im Fehlerfall
 */
int cdi_cache_sync(struct cdi_cache* cache);

/**
 * Cache-Block holen. Dabei wird intern ein Referenzzaehler erhoeht, sodass der
 * Block nicht freigegeben wird, solange er benutzt wird. Das heisst aber auch,
 * dass der Block nach der Benutzung wieder freigegeben werden muss, da sonst
 * die freien Blocks ausgehen.
 *
 * @param cache     Cache-Handle
 * @param blocknum  Blocknummer
 *
 * @return Pointer auf das Block-Handle oder NULL im Fehlerfall
 */
struct cdi_cache_block* cdi_cache_block_get(struct cdi_cache* cache,
    uint64_t blocknum);

/**
 * Cache-Block freigeben
 *
 * @param cache Cache-Handle
 * @param block Block-Handle
 */
void cdi_cache_block_release(struct cdi_cache* cache,
    struct cdi_cache_block* block);

/**
 * Cache-Block als veraendert markieren
 *
 * @param cache Cache-Handle
 * @param block Block-Handle
 */
void cdi_cache_block_dirty(struct cdi_cache* cache,
    struct cdi_cache_block* block);

/**
 * Cache-Eintrag erstellen
 *
 * @param offset    Position auf dem Datentraeger
 * @param size      Groesse des Bereichs
 *
 * @return Handle
 */
struct cdi_cache_entry* cdi_cache_entry_new(struct cdi_cache* cache,
    uint64_t offset, size_t size);

/**
 * Cache-Eintrag freigeben
 *
 * @param entry Handle
 */
void    cdi_cache_entry_release(struct cdi_cache_entry* entry);

/**
 * Cache-Eintrag sperren
 *
 * @param entry Handle
 */
void    cdi_cache_entry_lock(struct cdi_cache_entry* entry);

/**
 * Cache-Eintrag entsperren
 *
 * @param entry Handle
 */
void    cdi_cache_entry_unlock(struct cdi_cache_entry* entry);

/**
 * Cache-Eintrag als veraendert markieren
 *
 * @param entry Handle
 */
void    cdi_cache_entry_dirty(struct cdi_cache_entry* entry);

/**
 * Einzelnen Block im Cache-Eintrag als veraendert markieren
 *
 * @param entry Handle
 * @param block Blocknummer (von 0 an)
 */
void    cdi_cache_entry_blkdirty(struct cdi_cache_entry* entry, uint64_t block);



/**
 * Pointer auf einen Block im Cache-Eintrag holen
 *
 * @param entry Handle
 * @param block Blocknummer relativ zum Eintrag (der Offset innerhalb des
 *              Eintrags wird nicht beruecksichtigt)
 *
 * @return Pointer auf den Block
 */
void*   cdi_cache_entry_block(struct cdi_cache_entry* entry, size_t block);

/**
 * Daten aus dem Cache-Eintrag lesen
 *
 * @param entry     Handle
 * @param offset    Offset relativ zum Cache-Eintrag
 * @param size      Groesse des zu lesenden Bereichs
 * @param buffer    Puffer in dem die Daten abgelegt werden sollen
 */
int     cdi_cache_entry_read(struct cdi_cache_entry* entry, size_t offset,
            size_t size, void* buffer);

/**
 * Daten in den Cache-Eintrag schreiben
 *
 * @param entry     Handle
 * @param offset    Offset relativ zum Cache-Eintrag
 * @param size      Groesse des zu schreibenden Bereichs
 * @param buffer    Puffer aus dem die Daten gelesen werden
 */
int     cdi_cache_entry_write(struct cdi_cache_entry* entry, size_t offset,
            size_t size, const void* buffer);


#endif
