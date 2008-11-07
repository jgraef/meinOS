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

size_t ramdisk_fs_file_read(struct cdi_fs_stream* stream, uint64_t start,
    size_t size, void* data)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    // maximal bis Ende lesen.
    if ((res->size - start) < size) {
        size = res->size - start;
    }

    memcpy(data, res->buffer + start, size);
    return size;
}

size_t ramdisk_fs_file_write(struct cdi_fs_stream* stream, uint64_t start,
    size_t size, const void* data)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    // Es soll mehr geschrieben werden als Platz ist. Buffer vergroessern!.
    if ((res->size - start) < size) {
        void* new_buffer;
        size_t new_size;
        new_size = start + size;
        if (!(new_buffer = realloc(res->buffer,new_size))) {
            stream->error = CDI_FS_ERROR_INTERNAL;
            return 0;
        }
        res->buffer = new_buffer;
        res->size = new_size;
    }

    memcpy(res->buffer + start, data, size);
    return size;
}

int ramdisk_fs_file_truncate(struct cdi_fs_stream* stream, uint64_t size)
{
    struct ramdisk_fs_res* res = (struct ramdisk_fs_res*) stream->res;

    // Nichts zu tun
    if (size == res->size) {
        return 1;
    }

    void* new_buffer;
    if (!(new_buffer = realloc(res->buffer,size))) {
        stream->error = CDI_FS_ERROR_INTERNAL;
    }
    if (res->size < size) {
        memset(new_buffer + res->size,0,size - res->size);
    }

    res->buffer = new_buffer;
    res->size = size;
    return 1;
}
