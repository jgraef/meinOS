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

#ifndef _CLUSTER_H_
#define _CLUSTER_H_

#include <stddef.h>
#include <stddef.h>

struct fat_cluster_chain {
  size_t num_clusters;
  uint64_t *clusters;
};

struct fat_cluster_chain *fat_clchain_create(struct cdi_fs_filesystem *fs,size_t first_cluster);
uint64_t fat_cluster_read(struct cdi_fs_filesystem *fs,struct fat_cluster_chain *clchain,uint64_t start,uint64_t size,void *buf);

#endif
