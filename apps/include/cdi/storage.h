/*
    meinOS - A unix-like x86 microkernel operating system
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

#ifndef _CDI_STORAGE_H_
#define _CDI_STORAGE_H_

#include <sys/types.h>
#include <devfs.h>
#include <cdi.h>
#include <stdint.h>

struct cdi_storage_device {
  struct cdi_device dev;
  size_t block_size;
  uint64_t block_count;
  /// @note meinos specific
  uint64_t prevpos;
  void *buffer;
  uint64_t buffer_curblock;
  int buffer_loaded;
  int buffer_dirty;
  devfs_dev_t *devfs;
};

struct cdi_storage_driver {
  struct cdi_driver drv;
  /**
   * Reads blocks
   * @param start Blocknum of first block
   * @param count How many blocks to read
   * @param buffer Buffer where data is stored
   * @return 0 = Success; -1 = Failure
   */
  int (*read_blocks)(struct cdi_storage_device* device,uint64_t start,uint64_t count,void* buffer);
  /**
   * Writes blocks
   * @param start Blocknum of first block
   * @param count How many blocks to write
   * @param buffer Buffer to read data from
   * @return 0 = Success; -1 Failure
   */
  int (*write_blocks)(struct cdi_storage_device* device,uint64_t start,uint64_t count,void* buffer);
};

void cdi_storage_driver_init(struct cdi_storage_driver* driver);
void cdi_storage_driver_destroy(struct cdi_storage_driver* driver);
void cdi_storage_driver_register(struct cdi_storage_driver* driver);

#endif
