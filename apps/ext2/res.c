/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
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

#include "ext2_cdi.h"

/**
 * Handler fuer das Fuellen des Verzeichnisses mit ext2_dir_foreach
 */
static int dir_fill_handler(ext2_dirent_t* dirent, void* priv)
{
    struct ext2_fs_res* res = malloc(sizeof(*res));
    struct ext2_fs_res* parent_res = (struct ext2_fs_res*) priv;

    memset(res, 0, sizeof(*res));
    cdi_list_push(parent_res->res.children, res);
    res->res.parent = (struct cdi_fs_res*) parent_res;

    res->res.name = ext2_dir_alloc_name(dirent);
    res->res.res = &ext2_fs_res;
    res->inode_num = dirent->inode;
    return 0;
}

static int dir_fill(struct cdi_fs_stream* stream)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;
    res->res.children = cdi_list_create();

    ext2_dir_foreach(res->inode, dir_fill_handler, res);

    return 1;
}

static int dir_clear(struct cdi_fs_stream* stream)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;
    struct ext2_fs_res* child;

    while ((child = cdi_list_pop(res->res.children))) {
        free(child->res.name);
        free(child);
    }

    cdi_list_destroy(res->res.children);
    res->res.children = NULL;
    return 1;
}


int ext2_fs_res_load(struct cdi_fs_stream* stream)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;
    ext2_fs_t* fs = (ext2_fs_t*) stream->fs->opaque;

    if (res->res.loaded) {
        return 0;
    }

    // Ein neuer Kindeintrag dem noch keine Klasse zugewiesen wurde
    if (res->inode_num == 0) {
        res->res.loaded = 1;
    }

    res->inode = malloc(sizeof(ext2_inode_t));
    if (!ext2_inode_read(fs, res->inode_num, res->inode)) {
        free(res->inode);
        stream->error = CDI_FS_ERROR_IO;
        return 0;
    }

    switch (ext2_inode_type(res->inode)) {
        case EXT2_IT_FILE:
            res->res.file = &ext2_fs_file;
            break;

        case EXT2_IT_DIR:
            res->res.dir = &ext2_fs_dir;
            dir_fill(stream);
            break;

        case EXT2_IT_SYMLINK:
            res->res.link = &ext2_fs_link;
            res->res.link_path = ext2_symlink_read(res->inode);
            break;

        // TODO
        // Spezialdateien

        default:
            // Oops
            free(res->inode);
            stream->error = CDI_FS_ERROR_INTERNAL;
            return 0;
    }

    
    res->res.loaded = 1;
    return 1;
}

int ext2_fs_res_unload(struct cdi_fs_stream* stream)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;

    if (!res->res.loaded) {
        return 0;
    }

    if (res->res.dir) {
        dir_clear(stream);
    }

    free(res->inode);
    res->res.loaded = 0;
    return 1;
}

int64_t ext2_fs_res_meta_read(struct cdi_fs_stream* stream, cdi_fs_meta_t meta)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;
    ext2_fs_t* fs = (ext2_fs_t*) stream->fs->opaque;

    switch (meta) {
        case CDI_FS_META_SIZE:
            return res->inode->raw.size;

        case CDI_FS_META_USEDBLOCKS:
            return res->inode->raw.block_count;

        case CDI_FS_META_BLOCKSZ:
            return 512;

        case CDI_FS_META_BESTBLOCKSZ:
            return ext2_sb_blocksize(fs->sb);

        case CDI_FS_META_CREATETIME:
            return res->inode->raw.creation_time;

        case CDI_FS_META_ACCESSTIME:
            return res->inode->raw.access_time;

        case CDI_FS_META_CHANGETIME:
            return res->inode->raw.modification_time;
    }

    return 0;
}

int ext2_fs_res_meta_write(struct cdi_fs_stream* stream, cdi_fs_meta_t meta,
    int64_t value)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;
   
    switch (meta) {
        case CDI_FS_META_ACCESSTIME:
            res->inode->raw.access_time = value;
            return 1;

        case CDI_FS_META_CHANGETIME:
            res->inode->raw.modification_time = value;
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

int ext2_fs_res_assign_class(struct cdi_fs_stream* stream,
    cdi_fs_res_class_t class)
{
    struct ext2_fs_res* res = (struct ext2_fs_res*) stream->res;
    struct ext2_fs_res* parent_res = (struct ext2_fs_res*) res->res.parent;
    //ext2_fs_t* fs = (ext2_fs_t*) stream->fs->opaque;

    // In ext2 koennen die Ressource nur zu maximal einer Klasse gleichzeitig
    // gehoeren
    if (res->res.file || res->res.dir || res->res.link || res->res.special)
    {
        stream->error = CDI_FS_ERROR_ONS;
        return 0;
    }


    res->inode = malloc(sizeof(ext2_inode_t));
    switch (class) {
        case CDI_FS_CLASS_FILE:
            if (!ext2_file_create(parent_res->inode, res->res.name,
                res->inode))
            {
                stream->error = CDI_FS_ERROR_IO;
                goto error_out;
            }

            res->res.file = &ext2_fs_file;
            break;

        case CDI_FS_CLASS_DIR:
            if (!ext2_dir_create(parent_res->inode, res->res.name,
                res->inode))
            {
                stream->error = CDI_FS_ERROR_IO;
                goto error_out;
            }

            res->res.dir = &ext2_fs_dir;
            break;

        case CDI_FS_CLASS_LINK:
            if (!ext2_symlink_create(((struct ext2_fs_res*)
                res->res.parent)->inode, res->res.name, "", res->inode))
            {
                stream->error = CDI_FS_ERROR_IO;
                goto error_out;
            }

            res->res.link = &ext2_fs_link;
            break;

        case CDI_FS_CLASS_SPECIAL:
            stream->error = CDI_FS_ERROR_NOT_IMPLEMENTED;
            goto error_out;
            break;

    };

    res->inode_num = res->inode->number;
    return 1;

error_out:
    free(res->inode);
    res->inode = NULL;
    return 0;
}

