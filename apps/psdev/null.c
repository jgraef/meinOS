#include <sys/types.h>
#include <devfs.h>

ssize_t null_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return 0;
}

ssize_t null_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return count;
}

int null_init() {
  devfs_dev_t *dev;
  dev = devfs_createdev("null");
  if (dev!=NULL) {
    devfs_onread(dev,null_read);
    devfs_onwrite(dev,null_write);
    return 0;
  }
  else return -1;
}
