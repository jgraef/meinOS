#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cdi/storage.h>
#include <devfs.h>
#include <stdint.h>
#include <stddef.h>

/**
 * Writes old block to disk and loads new block to buffer
 *  @param device CDI Storage Device
 *  @param newpos New position to load
 *  @return Success?
 */
static int cdi_storage_swapbuffer(struct cdi_storage_device* device,uint64_t newpos) {
  struct cdi_storage_driver *driver = (struct cdi_storage_driver*)(device->dev.driver);
  if (device->buffer==NULL) device->buffer = malloc(device->block_size);
  if (device->buffer_curblock!=newpos/device->block_size || !device->buffer_loaded) {
    if (device->buffer_loaded && driver->write_blocks!=NULL && device->buffer_dirty) {
      if (driver->write_blocks(device,device->buffer_curblock/device->block_size,1,device->buffer)==-1) return -1;
    }
    if (driver->read_blocks!=NULL) {
      if (driver->read_blocks(device,newpos/device->block_size,1,device->buffer)==-1) return -1;
      device->buffer_dirty = 0;
      device->buffer_loaded = 1;
    }
    device->buffer_curblock = newpos/device->block_size;
  }
  return 0;
}

/**
 * Reads from storage via Callback
 *  @param buffer Buffer to store data in
 *  @param count How many bytes to read
 *  @param dev DevFS device
 *  @return How many bytes read
 */
static int cdi_storage_read(devfs_dev_t *dev,void *buffer,size_t count,off_t pos) {
  fprintf(stderr,"cdi_storage_read(0x%x,0x%x,0x%x,0x%x)\n",dev,buffer,count,pos);
  struct cdi_storage_device* device = dev->user_data;
  size_t i;

  //if (pos>device->block_count*device->block_size) return -1;
  //if (pos+count>device->block_count*device->block_size) count = device->block_count*device->block_size-pos;

  uint64_t offset = pos%device->block_size;
  uint64_t blockpos = pos-offset;
  size_t blocks = (count+offset-1)/device->block_size+1;

  for (i=0;i<blocks;i++) {
    size_t count_cur = device->block_size-offset;
    count_cur = count_cur>count?count:count_cur;

    cdi_storage_swapbuffer(device,blockpos);
    memcpy(buffer+i*device->block_size,device->buffer+offset,count_cur);

    blockpos += device->block_size;
    offset = 0;
  }

  return count;
}

/**
 * Writes to storage via Callback
 *  @param buffer Buffer to read data from
 *  @param count How many bytes to write
 *  @param dev DevFS device
 *  @return How many bytes written
 */
static int cdi_storage_write(devfs_dev_t *dev,void *buffer,size_t count,off_t pos) {
  struct cdi_storage_device* device = dev->user_data;
  size_t i;

  //if (pos>device->block_count*device->block_size) return -1;
  //if (pos+count>device->block_count*device->block_size) count = device->block_count*device->block_size-pos;

  uint64_t offset = pos%device->block_size;
  uint64_t blockpos = pos-offset;
  size_t blocks = (count+offset-1)/device->block_size+1;

  for (i=0;i<blocks;i++) {
    size_t count_cur = device->block_size-offset;
    count_cur = count_cur>count?count:count_cur;

    cdi_storage_swapbuffer(device,blockpos);
    memcpy(buffer+i*device->block_size,device->buffer+offset,count_cur);
    device->buffer_dirty = 1;

    blockpos += device->block_size;
    offset = 0;
  }

  return count;
}

/**
 * Initializes CDI Storage Driver
 *  @param driver CDI Storage Driver
 */
void cdi_storage_driver_init(struct cdi_storage_driver* driver) {
  cdi_driver_init((struct cdi_driver*)driver);
}

/**
 * Destroys CDI Storage Driver
 *  @param driver CDI Storage Driver
 */
void cdi_storage_driver_destroy(struct cdi_storage_driver* driver) {
  struct cdi_storage_device* device;
  uint32_t i;

  for (i=0;(device = cdi_list_get(driver->drv.devices,i));i++) {
    devfs_removedev(device->devfs);
    free(device->buffer);
  }
  cdi_driver_destroy((struct cdi_driver*)driver);
}

/**
 * Registers CDI Storage Driver
 *  @param driver CDI Storage Driver
 *  @todo Fixme: Create device later. After driver->init() call
 */
void cdi_storage_driver_register(struct cdi_storage_driver* driver) {
  struct cdi_storage_device* device;
  uint32_t i;

  for (i=0;(device = cdi_list_get(driver->drv.devices,i));i++) {
    device->devfs = devfs_createdev(device->dev.name);
    if (device->devfs!=NULL) {
      device->devfs->user_data = device;
      devfs_onread(device->devfs,cdi_storage_read);
      devfs_onwrite(device->devfs,cdi_storage_write);
    }
    device->buffer = NULL;
    device->buffer_curblock = 0;
    device->buffer_loaded = 0;
    device->buffer_dirty = 0;
  }
  cdi_driver_register((struct cdi_driver*)driver);
}
