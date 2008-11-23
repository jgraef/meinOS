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

#include <stdint.h>

#include "fat_cdi.h"
#include "cluster.h"

#include "cdi/fs.h"

/**
 * Reads from file
 *  @param stream CDI FS stream
 *  @param start Offset in flie
 *  @param size How many bytes to read
 *  @param buffer Buffer to store data in
 *  @return How many bytes read
 */
size_t fat_fs_file_read(struct cdi_fs_stream *stream,uint64_t start,size_t size,void *buffer) {
  debug("fat_fs_file_read(0x%x,0x%x,0x%x,0x%x)\n",stream,start,size,buffer);
  struct fat_fs_res *res = (struct fat_fs_res*)stream->res;

  if (start>res->filesize) return 0;
  if (start+size>res->filesize) size = res->filesize-start;

  fat_cluster_read(res->fs,res->clusters,start,size,buffer);
  return size;
}

/**
 * Writes to file
 *  @param stream CDI FS stream
 *  @param start Offset in flie
 *  @param size How many bytes to write
 *  @param buffer Data to write to file
 *  @return How many bytes read
 */
size_t fat_fs_file_write(struct cdi_fs_stream *stream,uint64_t start,size_t size,const void *buffer) {
  debug("fat_fs_file_write(0x%x,0x%x,0x%x,0x%x)\n",stream,start,size,buffer);
  struct fat_fs_res *res = (struct fat_fs_res*)stream->res;

  if (stream->fs->read_only) return 0;

  if (start+size>res->filesize) {
    /// @todo
    //fat_fs_file_truncate(stream,start+size);
    return 0;
  }

  fat_cluster_write(res->fs,res->clusters,start,size,buffer);
  return size;
}

