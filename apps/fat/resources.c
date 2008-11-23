/*
 * fat - A FAT* CDI driver
 *
 * Copyright (C) 2008 Janosch Gräf
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "fat_cdi.h"

struct cdi_fs_res_res fat_fs_res_res = {
    .load = fat_fs_res_load,
    .unload = fat_fs_res_unload,
    //.meta_read = fat_fs_res_meta_read
};

struct cdi_fs_res_file fat_fs_res_file = {
    // Prinzipiell haben wir nur ausfuehrbare Dateien, der Rest wird mit den
    // Berechtigungen geregelt
    .executable = 1,

    .read = fat_fs_file_read,
    .write = fat_fs_file_write
};

struct cdi_fs_res_dir fat_fs_res_dir = {
    .list = fat_fs_dir_list
};
