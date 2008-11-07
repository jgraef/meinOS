#include <sys/types.h>
#include <devfs.h>

ssize_t zero_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  memset(buf,0,count);
  return count;
}

ssize_t zero_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return 0;
}

int zero_init() {
  devfs_dev_t *dev;
  dev = devfs_createdev("zero");
  if (dev!=NULL) {
    devfs_onread(dev,zero_read);
    devfs_onwrite(dev,zero_write);
    return 0;
  }
  else return -1;
}
