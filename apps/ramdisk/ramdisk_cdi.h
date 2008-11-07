/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Alexander Siol and Janosch Graef.
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

#ifndef _RAMDISK_CDI_H_
#define _RAMDISK_CDI_H_

#include <stdint.h>
#include <stdlib.h>

#include "cdi/fs.h"


/**
 * Dateisystemressource fuer ramdisk
 */
struct ramdisk_fs_res {
    struct cdi_fs_res res;

    // Buffer fuer die Daten, und dessen Groesse
    void* buffer;
    size_t size;

    // Zugriffszeiten (Meta-Informationen)
    int64_t creation_time;
    int64_t access_time;
    int64_t modification_time;
};


// CDI-IF
int ramdisk_fs_init(struct cdi_fs_filesystem* fs);
int ramdisk_fs_destroy(struct cdi_fs_filesystem* fs);

// CDI Res
int     ramdisk_fs_res_load(struct cdi_fs_stream* stream);
int     ramdisk_fs_res_unload(struct cdi_fs_stream* stream);
int     ramdisk_fs_res_remove(struct cdi_fs_stream* stream);
int     ramdisk_fs_res_rename(struct cdi_fs_stream* stream, const char* name);
//int     ramdisk_fs_res_move(struct cdi_fs_stream* stream, struct cdi_fs_res* dest);
int     ramdisk_fs_res_assign_class(struct cdi_fs_stream* stream,
            cdi_fs_res_class_t class);
int     ramdisk_fs_res_remove_class(struct cdi_fs_stream* stream,
            cdi_fs_res_class_t class);
int64_t ramdisk_fs_res_meta_read(struct cdi_fs_stream* stream, cdi_fs_meta_t meta);
int     ramdisk_fs_res_meta_write(struct cdi_fs_stream* stream, cdi_fs_meta_t meta,
    int64_t value);
int ramdisk_fs_res_destroy(struct ramdisk_fs_res* res);

// CDI File
size_t  ramdisk_fs_file_read(struct cdi_fs_stream* stream, uint64_t start,
            size_t size, void* data);
size_t  ramdisk_fs_file_write(struct cdi_fs_stream* stream, uint64_t start,
            size_t size, const void* data);
int     ramdisk_fs_file_truncate(struct cdi_fs_stream* stream, uint64_t size);

// CDI Dir
cdi_list_t  ramdisk_fs_dir_list(struct cdi_fs_stream* stream);
int         ramdisk_fs_dir_create_child(struct cdi_fs_stream* stream,
                const char* name, struct cdi_fs_res* parent);

// CDI Link
const char* ramdisk_fs_link_read(struct cdi_fs_stream* stream);
int         ramdisk_fs_link_write(struct cdi_fs_stream* stream, const char* path);


// ramdisk-Ressourcen(-Typen)
extern struct cdi_fs_res_res    ramdisk_fs_res;
extern struct cdi_fs_res_file   ramdisk_fs_file;
extern struct cdi_fs_res_dir    ramdisk_fs_dir;
extern struct cdi_fs_res_link   ramdisk_fs_link;

#endif

