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
#include <string.h>

#include "cdi/cache.h"

#include "fat_cdi.h"

/**
 * Reads data from device for cache
 *  @param cache CDI cache
 *  @param block Block to read
 *  @param count How many blocks to read
 *  @param dest Buffer to store data in
 *  @param prv Private data (CDI filesystem)
 *  @return How many sectors read
 */
int fat_sector_read_cache(struct cdi_cache *cache,uint64_t block,size_t count,void *dest,void *prv) {
  debug("fat_sector_read_cache(0x%x,0x%x,0x%x,0x%x,0x%x)\n",cache,block,count,dest,prv);
  struct fat_fs_filesystem *fat_fs = ((struct cdi_fs_filesystem*)prv)->opaque;
  uint64_t start = block*fat_fs->bootsector->sector_size;
  size_t size = count*fat_fs->bootsector->sector_size;
  return cdi_fs_data_read(prv,start,size,dest)/fat_fs->bootsector->sector_size;
}

/**
 * Writes data to device for cache
 *  @param cache CDI cache
 *  @param block Block to write
 *  @param count How many blocks to write
 *  @param dest Data to write
 *  @param prv Private data (CDI filesystem)
 *  @return How many sectors written
 */
int fat_sector_write_cache(struct cdi_cache *cache,uint64_t block,size_t count,const void *dest,void *prv) {
  debug("fat_sector_write_cache(0x%x,0x%x,0x%x,0x%x,0x%x)\n",cache,block,count,dest,prv);
  struct fat_fs_filesystem *fat_fs = ((struct cdi_fs_filesystem*)prv)->opaque;
  uint64_t start = block*fat_fs->bootsector->sector_size;
  size_t size = count*fat_fs->bootsector->sector_size;
  return cdi_fs_data_write(prv,start,size,dest)/fat_fs->bootsector->sector_size;
}

/**
 * Reads data from volume (NO CLUSTER READ)
 *  @param fs Filesystem
 *  @param pos Position on volume
 *  @param size How many bytes to read
 *  @param Buffer Buffer to store data in
 *  @return How many bytes read
 */
size_t fat_read(struct cdi_fs_filesystem *fs,uint64_t pos,size_t size,void *buffer) {
  struct fat_fs_filesystem *fat_fs = fs->opaque;
  size_t block = pos/fat_fs->bootsector->sector_size;
  if (block>=fat_num_sectors(fat_fs->bootsector)) return 0;
  size_t offset = pos%fat_fs->bootsector->sector_size;
  size_t rem_size = size;

  while (rem_size>0) {
    struct cdi_cache_block *cache_block = cdi_cache_block_get(fat_fs->cache,block++,0);
    size_t cur_size = rem_size>fat_fs->bootsector->sector_size?fat_fs->bootsector->sector_size:rem_size;
    memcpy(buffer,cache_block->data+offset,cur_size);
    cdi_cache_block_release(fat_fs->cache,cache_block);
    buffer += cur_size;
    rem_size -= cur_size;
    offset = 0;
  }

  return size;
}

/**
 * Writes data to volume (NO CLUSTER WRITE)
 *  @param fs Filesystem
 *  @param pos Position on volume
 *  @param size How many bytes to write
 *  @param Buffer Data to write to volume
 *  @return How many bytes written
 */
size_t fat_write(struct cdi_fs_filesystem *fs,uint64_t pos,size_t size,const void *buffer) {
  struct fat_fs_filesystem *fat_fs = fs->opaque;
  size_t block = pos/fat_fs->bootsector->sector_size;
  if (block>=fat_num_sectors(fat_fs->bootsector)) return 0;
  size_t offset = pos%fat_fs->bootsector->sector_size;
  size_t rem_size = size;

  while (rem_size>0) {
    struct cdi_cache_block *cache_block = cdi_cache_block_get(fat_fs->cache,block++,0);
    size_t cur_size = rem_size>fat_fs->bootsector->sector_size?fat_fs->bootsector->sector_size:rem_size;
    memcpy(cache_block->data+offset,buffer,cur_size);
    cdi_cache_block_release(fat_fs->cache,cache_block);
    buffer += cur_size;
    rem_size -= cur_size;
    offset = 0;
  }

  return size;
}
