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

#ifndef _FAT_CDI_H_
#define _FAT_CDI_H_

#include <stddef.h>
#include <stdint.h>

#include "cdi/fs.h"
#include "cdi/cache.h"

#include "bootsector.h"

#define FAT_LOWER_FILENAMES

#define DEBUG stderr

/// FAT type
typedef enum {
  FAT12 = 12,
  FAT16 = 16,
  FAT32 = 32
} fat_type_t;

/// FAT filesystem
struct fat_fs_filesystem {
  /// Boot sector
  struct fat_bootsector *bootsector;

  /// FAT type (FAT12, FAT16 or FAT32)
  fat_type_t type;

  /// Data area (Cluster area)
  uint64_t data_area;

  /// Cache
  struct cdi_cache *cache;
};

/// FAT resource
struct fat_fs_res {
  struct cdi_fs_res res;

  /// FAT filesystem
  struct cdi_fs_filesystem *fs;

  /// Resource class
  cdi_fs_res_class_t class;

  /// Extended data (e.g. root dir on FAT12/26)
  uint64_t ext_data;

  /// File size
  size_t filesize;

  /// Read only
  int readonly;

  /// Cluster chain
  struct fat_cluster_chain *clusters;
};

// sector.c
int fat_sector_read_cache(struct cdi_cache *cache,uint64_t block,size_t count,void *dest,void *prv);
int fat_sector_write_cache(struct cdi_cache *cache,uint64_t block,size_t count,const void *dest,void *prv);
size_t fat_read(struct cdi_fs_filesystem *fs,uint64_t pos,size_t size,void *buffer);
size_t fat_write(struct cdi_fs_filesystem *fs,uint64_t pos,size_t size,const void *buffer);

// init.c
int fat_fs_init(struct cdi_fs_filesystem *fs);
int fat_fs_destroy(struct cdi_fs_filesystem *fs);

// res.c
struct fat_fs_res* fat_fs_res_create(const char *name,struct fat_fs_res *parent,cdi_fs_res_class_t class,cdi_fs_res_type_t type);
int fat_fs_res_destroy(struct fat_fs_res *res);
int fat_fs_res_load(struct cdi_fs_stream *stream);
int fat_fs_res_unload(struct cdi_fs_stream *stream);

// file.c
size_t fat_fs_file_read(struct cdi_fs_stream *stream,uint64_t start,size_t size,void *buffer);
size_t fat_fs_file_write(struct cdi_fs_stream *stream,uint64_t start,size_t size,const void *buffer);

// dir.c
cdi_list_t fat12_16_rootdir_load(struct cdi_fs_filesystem *fs);
cdi_list_t fat_dir_load(struct fat_fs_res *res);
cdi_list_t fat_fs_dir_list(struct cdi_fs_stream *stream);

// resources.c
extern struct cdi_fs_res_res    fat_fs_res_res;
extern struct cdi_fs_res_file   fat_fs_res_file;
extern struct cdi_fs_res_dir    fat_fs_res_dir;

// Debug
int debug(const char *fmt,...);

#endif
