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

#include "ramdisk_cdi.h"

int ramdisk_fs_res_load(struct cdi_fs_stream* stream)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    if (res->res.loaded) {
        return 0;
    }

    res->res.loaded = 1;
    return 1;
}

int ramdisk_fs_res_unload(struct cdi_fs_stream* stream)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    if (!res->res.loaded) {
        return 0;
    }

    res->res.loaded = 0;
    return 1;
}

int64_t ramdisk_fs_res_meta_read(struct cdi_fs_stream* stream, cdi_fs_meta_t meta)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    switch (meta) {
        case CDI_FS_META_SIZE:
            return res->size;

        case CDI_FS_META_USEDBLOCKS:
            return 1;

        case CDI_FS_META_BLOCKSZ:
            return res->size;

        case CDI_FS_META_BESTBLOCKSZ:
            return res->size;

        case CDI_FS_META_CREATETIME:
            return res->creation_time;

        case CDI_FS_META_ACCESSTIME:
            return res->access_time;

        case CDI_FS_META_CHANGETIME:
            return res->modification_time;
    }

    return 0;
}

int ramdisk_fs_res_meta_write(struct cdi_fs_stream* stream, cdi_fs_meta_t meta,
                           int64_t value)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    switch (meta) {
        case CDI_FS_META_ACCESSTIME:
            res->access_time = value;
            return 1;

        case CDI_FS_META_CHANGETIME:
            res->modification_time = value;
            return 1;

        // RO:
        case CDI_FS_META_SIZE:
        case CDI_FS_META_USEDBLOCKS:
        case CDI_FS_META_BESTBLOCKSZ:
        case CDI_FS_META_BLOCKSZ:
        case CDI_FS_META_CREATETIME:
            return 0;
    }

    return 0;
}

int ramdisk_fs_res_assign_class(struct cdi_fs_stream* stream,
                             cdi_fs_res_class_t class)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    // In ramdisk koennen die Ressource nur zu maximal einer Klasse gleichzeitig
    // gehoeren
    if (res->res.file || res->res.dir || res->res.link || res->res.special)
    {
        stream->error = CDI_FS_ERROR_ONS;
        return 0;
    }

    switch (class) {
        case CDI_FS_CLASS_FILE:
            res->res.file = &ramdisk_fs_file;
            break;

        case CDI_FS_CLASS_DIR:
            res->res.dir = &ramdisk_fs_dir;
            break;

        case CDI_FS_CLASS_LINK:
            res->res.link = &ramdisk_fs_link;
            break;

        case CDI_FS_CLASS_SPECIAL:
            stream->error = CDI_FS_ERROR_NOT_IMPLEMENTED;
            return 0;
            break;

    };

    return 1;
}

int ramdisk_fs_res_remove_class(struct cdi_fs_stream* stream,
            cdi_fs_res_class_t class)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    switch (class) {
        case CDI_FS_CLASS_FILE:
            res->size = 0;
            res->res.file = NULL;
            break;

        case CDI_FS_CLASS_DIR:
            res->res.dir = NULL;
            break;

        case CDI_FS_CLASS_LINK:
            res->res.link = NULL;
            break;

        case CDI_FS_CLASS_SPECIAL:
            res->res.special = NULL;
            break;
    };

    return 1;
}

int ramdisk_fs_res_rename(struct cdi_fs_stream* stream, const char* name)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;
    free(res->res.name);
    res->res.name = strdup(name);
    return 1;
}

int ramdisk_fs_res_remove(struct cdi_fs_stream* stream)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    if (res->res.file!=NULL || res->res.dir!=NULL || res->res.link!=NULL || res->res.special!=NULL) {
        stream->error = CDI_FS_ERROR_ONS;
        return 0;
    }

    // Link aus der Liste der Vater-Resource entfernen
    size_t i;
    struct ramdisk_fs_res* child;
    for (i=0;(child = cdi_list_get(res->res.parent->children,i));i++) {
        if (child==res) {
            cdi_list_remove(res->res.parent->children,i);
            break;
        }
    }

    return ramdisk_fs_res_destroy(res);
}

int ramdisk_fs_res_destroy(struct ramdisk_fs_res* res)
{
    if (cdi_list_size(res->res.children)>0) return 0;

    free(res->buffer);
    free(res->res.name);
    free(res->res.link_path);
    cdi_list_destroy(res->res.children);
    free(res);

    return 1;
}
