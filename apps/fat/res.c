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

#include <string.h>

#include "fat_cdi.h"

#include "cdi/fs.h"
#include "cdi/lists.h"

/**
 * Creates a new resource
 *  @param name Name of resource
 *  @param parent Parent resource
 *  @param class File class
 *  @param type Special file type
 *  @return FAT class
 */
struct fat_fs_res* fat_fs_res_create(const char *name,struct fat_fs_res *parent,cdi_fs_res_class_t class,cdi_fs_res_type_t type) {
  debug("fat_fs_res_create(%s,0x%x,%d,%d)\n",name,parent,class,type);

  struct fat_fs_res *res = malloc(sizeof(struct fat_fs_res));
  res->res.name = strdup(name);
  res->class = class;
  res->res.type = type;
  if (parent!=NULL) res->fs = parent->fs;

  res->res.res = &fat_fs_res_res;
  if (class==CDI_FS_CLASS_FILE) res->res.file = &fat_fs_res_file;
  else if (class==CDI_FS_CLASS_DIR) res->res.dir = &fat_fs_res_dir;

  return res;
}

/**
 * Destroys a resource
 *  @param res FAT resource
 *  @return 0=success; -1=failure
 */
int fat_fs_res_destroy(struct fat_fs_res *res) {
  debug("fat_fs_res_destroy(0x%x)\n",res);

  free(res->res.name);
  if (res->res.children!=NULL) {
    size_t i;
    struct fat_fs_res *child;
    for (i=0;(child = cdi_list_get(res->res.children,i));i++) fat_fs_res_destroy(child);
    cdi_list_destroy(res->res.children);
  }
  return 0;
}

/**
 * Loads a resource
 *  @param stream CDI FS stream
 *  @return 0=success; -1=failure
 */
int fat_fs_res_load(struct cdi_fs_stream *stream) {
  debug("fat_fs_res_load(0x%x)\n",stream);
  struct fat_fs_res *res = (struct fat_fs_res*)stream->res;

  if (!res->res.loaded) {
    if (res->class==CDI_FS_CLASS_DIR) res->res.children = fat_dir_load(res);
    else res->res.children = cdi_list_create();

    res->res.loaded = 1;
  }

  return 1;
}

/**
 * Unloads a resource
 *  @param stream CDI FS stream
 *  @return 0=success; -1=failure
 */
int fat_fs_res_unload(struct cdi_fs_stream *stream) {
  debug("fat_fs_res_unload(0x%x)\n",stream);
  struct fat_fs_res *res = (struct fat_fs_res*)stream->res;
  struct fat_fs_filesystem *fat_fs = res->fs->opaque;

  if (res->res.loaded) {
    // Destroy children
    struct fat_fs_res *child;
    while ((child = cdi_list_pop(res->res.children))) fat_fs_res_destroy(child);
    cdi_list_destroy(res->res.children);

    // Sync with colume
    cdi_cache_sync(fat_fs->cache);

    res->res.loaded = 0;
  }
  return 1;
}

/**
 * Reads meta data from resource
 *  @param stream CDI FS stream
 *  @param meta Type of meta data
 *  @return Meta data
 */
int64_t fat_fs_res_meta_read(struct cdi_fs_stream *stream,cdi_fs_meta_t meta) {
  struct fat_fs_res *res = (struct fat_fs_res*)stream->res;
  struct fat_fs_filesystem *fat_fs = res->fs->opaque;

  switch (meta) {
    case CDI_FS_META_SIZE:
      return res->filesize;

    case CDI_FS_META_USEDBLOCKS:
      return (res->filesize-1)/fat_fs->bootsector->sector_size*fat_fs->bootsector->cluster_size;

    case CDI_FS_META_BESTBLOCKSZ:
    case CDI_FS_META_BLOCKSZ:
      return fat_fs->bootsector->sector_size*fat_fs->bootsector->cluster_size;

    case CDI_FS_META_CREATETIME:
      return 0;

    case CDI_FS_META_ACCESSTIME:
      return 0;

    case CDI_FS_META_CHANGETIME:
      return 0;
  }
  return 0;
}
