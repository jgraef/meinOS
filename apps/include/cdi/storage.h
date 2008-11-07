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
