/*  
 * Copyright (c) 2007 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Antoine Kaufmann.
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

#include "libpartition.h"
#include <stdint.h>

/**
 * Roher Partitionstabelleneintrag
 */
struct raw_entry {
    uint8_t active;
    uint8_t begin_chs[3];
    uint8_t type;
    uint8_t end_chs[3];
    uint32_t start;
    uint32_t size;
} PACKED;

/**
 * Uebergebene Partitionstabelle anhand des Sektors fuellen
 *
 * @return 1 wenn die Tabelle erfolgreich gefuellt wurde, 0 sonst
 */
int partition_table_fill(struct partition_table* table, void* sector)
{
    struct raw_entry* entry = (struct raw_entry*) (sector +
        PARTITION_TABLE_OFFSET);
    uint16_t* signature = sector + PARTITION_TABLE_SIG_OFFSET;
    int i;

    // Zuerst die Signatur pruefen, denn ohne die muss garnicht weiter gesucht
    // werden.
    if (*signature != PARTITION_TABLE_SIGNATURE) {
        return 0;
    }
    
    // Wenn die Signatur existiert koennen die einzelnen Eintraege ausgelesen
    // werden
    for (i = 0; i < 4; i++) {
        // Wenn 0 als Groesse eingetragen ist, ist der Eintrag unbenutzt
        if (entry->size == 0) {
            table->entries[i].used = 0;
        } else {
            table->entries[i].used = 1;

            // Fuer erweiterte Partitionen wollen wir generell Typ 0x5, die
            // anderen aus Windows und Linux werden deshalb ausgetauscht.
            if ((entry->type == 0x0F) || (entry->type == 0x85)) {
                table->entries[i].type = PARTITION_TYPE_EXTENDED;
            } else {
                table->entries[i].type = entry->type;
            }

            table->entries[i].start = entry->start;
            table->entries[i].size = entry->size;
        }
        entry++;
    }
    return 1;
}

