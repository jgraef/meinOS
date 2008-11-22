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

#include <stdlib.h>
#include <string.h>

#include "fat_cdi.h"
#include "bootsector.h"
#include "dirent.h"
#include "cluster.h"

static fat_type_t fat_type(struct fat_bootsector *bootsector) {
  size_t rootdir_sectors = (bootsector->rootdir_max_ent*sizeof(struct fat_dirent)+(bootsector->sector_size-1))/bootsector->sector_size;
  size_t data_sectors = fat_num_sectors(bootsector)-(bootsector->reserved_sectors+rootdir_sectors+bootsector->num_fats*fat_fat_sectors(bootsector));
  size_t num_clusters = data_sectors/bootsector->cluster_size;
  if (num_clusters<4085) return FAT12;
  else if (num_clusters<65525) return FAT16;
  else return FAT32;
}

int fat_fs_init(struct cdi_fs_filesystem *fs) {
  debug("fat_fs_init(0x%x)\n",fs);

  struct fat_fs_filesystem *fat_fs = malloc(sizeof(struct fat_fs_filesystem));

  fs->opaque = fat_fs;
  // load boot sector
  if ((fat_fs->bootsector = fat_bootsector_load(fs))!=NULL) {
    // get FAT type
    fat_fs->type = fat_type(fat_fs->bootsector);
    debug("fat: type: FAT%d\n",fat_fs->type);

    // create cache
    fat_fs->cache = cdi_cache_create(fat_fs->bootsector->sector_size,0,fat_sector_read_cache,NULL,fs);

    // create root directory
    struct fat_fs_res *root_res = fat_fs_res_create("/",NULL,CDI_FS_CLASS_DIR,0);
    root_res->fs = fs;
    if (fat_fs->type==FAT12 || fat_fs->type==FAT16) {
      root_res->ext_data = fat_fat_start(fat_fs->bootsector,fat_fs->bootsector->num_fats);
      fat_fs->data_area = root_res->ext_data+fat_fs->bootsector->rootdir_max_ent*sizeof(struct fat_dirent);
    }
    else {
      root_res->clusters = fat_clchain_create(fs,fat_fs->bootsector->fat32.root_cluster);
      fat_fs->data_area = fat_fat_start(fat_fs->bootsector,fat_fs->bootsector->num_fats);
    }
    root_res->filesize = 0;
    fs->root_res = (struct cdi_fs_res*)root_res;

    return 1;
  }
  free(fat_fs);
  return 0;
}

int fat_fs_destroy(struct cdi_fs_filesystem *fs) {
  debug("fat_fs_destroy(0x%x)\n",fs);

  struct fat_fs_filesystem *fat_fs = fs->opaque;
  free(fat_fs->bootsector);
  free(fat_fs);
  return 0;
}
