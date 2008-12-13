/*
    fat - A FAT* CDI driver
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "bootsector.h"
#include "cluster.h"

struct fat_cluster_chain *fat_clchain_create(struct cdi_fs_filesystem *fs,size_t first_cluster) {
  struct fat_fs_filesystem *fat_fs = fs->opaque;
  struct fat_cluster_chain *clchain = malloc(sizeof(struct fat_cluster_chain));
  size_t cur_size = 0;
  size_t cur_cluster = first_cluster;
  uint64_t fat_start = fat_fat_start(fat_fs->bootsector,0);
  int last = 0;

  clchain->num_clusters = 0;
  clchain->clusters = NULL;

  while (!last) {
    if (clchain->num_clusters>=cur_size) {
      cur_size += 32;
      clchain->clusters = realloc(clchain->clusters,cur_size*sizeof(uint64_t));
    }

    clchain->clusters[clchain->num_clusters++] = fat_fs->data_area+((uint64_t)cur_cluster-2)*fat_fs->bootsector->cluster_size*fat_fs->bootsector->sector_size;

    size_t next_cluster;
    if (fat_fs->type==FAT12) {
      uint16_t ent;
      fat_read(fs,fat_start+cur_cluster*3/2,2,&ent);
      if (cur_cluster%2==1) ent = (ent>>4);
      ent &= 0xFFF;
      if (ent<0x3 || ent>0xFF7) last = 1;
      next_cluster = ent;
    }
    else if (fat_fs->type==FAT16) {
      uint16_t ent;
      fat_read(fs,fat_start+cur_cluster*2,2,&ent);
      if (ent<0x3 || ent>0xFFF7) last = 1;
      next_cluster = ent;
    }
    else if (fat_fs->type==FAT32) {
      uint32_t ent;
      fat_read(fs,fat_start+cur_cluster*4,4,&ent);
      ent &= 0xFFFFFFF;
      if (ent<0x3 || ent>0xFFFFFF7) last = 1;
      next_cluster = ent;
    }

    cur_cluster = next_cluster;
  }

  return clchain;
}

uint64_t fat_cluster_read(struct cdi_fs_filesystem *fs,struct fat_cluster_chain *clchain,uint64_t start,uint64_t size,void *buf) {
  struct fat_fs_filesystem *fat_fs = fs->opaque;
  size_t cluster_size = fat_fs->bootsector->cluster_size*fat_fs->bootsector->sector_size;
  size_t cluster_num = start/cluster_size;
  size_t offset = start%cluster_size;
  uint64_t rem_size = size;

  while (rem_size>0) {
    size_t cur_size = cluster_size-offset;
    if (cur_size>rem_size) cur_size = rem_size;
    fat_read(fs,clchain->clusters[cluster_num]+offset,cur_size,buf);
    buf += cur_size;
    rem_size -= cur_size;
    cluster_num++;
    offset = 0;
  }

  return size;
}

uint64_t fat_cluster_write(struct cdi_fs_filesystem *fs,struct fat_cluster_chain *clchain,uint64_t start,uint64_t size,const void *buf) {
  struct fat_fs_filesystem *fat_fs = fs->opaque;
  size_t cluster_size = fat_fs->bootsector->cluster_size*fat_fs->bootsector->sector_size;
  size_t cluster_num = start/cluster_size;
  size_t offset = start%cluster_size;
  uint64_t rem_size = size;

  while (rem_size>0) {
    size_t cur_size = cluster_size-offset;
    if (cur_size>rem_size) cur_size = rem_size;
    fat_write(fs,clchain->clusters[cluster_num]+offset,cur_size,buf);
    buf += cur_size;
    rem_size -= cur_size;
    cluster_num++;
    offset = 0;
  }

  return size;
}
