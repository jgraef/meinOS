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

struct cdi_fs_res_res   ramdisk_fs_res = {
    .load = ramdisk_fs_res_load,
    .unload = ramdisk_fs_res_unload,

    .meta_read = ramdisk_fs_res_meta_read,
    .meta_write = ramdisk_fs_res_meta_write,

    .assign_class = ramdisk_fs_res_assign_class,
    .remove_class = ramdisk_fs_res_remove_class,

    .rename = ramdisk_fs_res_rename,
    .remove = ramdisk_fs_res_remove
};

struct cdi_fs_res_file  ramdisk_fs_file = {
    // Prinzipiell haben wir nur ausfuehrbare Dateien, der Rest wird mit den
    // Berechtigungen geregelt
    .executable = 1,

    .read = ramdisk_fs_file_read,
    .write = ramdisk_fs_file_write,
    .truncate = ramdisk_fs_file_truncate
};

struct cdi_fs_res_dir   ramdisk_fs_dir = {
    .list = ramdisk_fs_dir_list,
    .create_child = ramdisk_fs_dir_create_child
};

struct cdi_fs_res_link  ramdisk_fs_link = {
    .read_link = ramdisk_fs_link_read,
    .write_link = ramdisk_fs_link_write
};

