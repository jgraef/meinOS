#include <sys/types.h>
#include <devfs.h>

ssize_t time_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  count = count>sizeof(time_t)?sizeof(time_t):count;
  time_t time_buf = time();
  memcpy(buf,&time_buf,count);
}

ssize_t time_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return 0;
}

int time_init() {
  devfs_dev_t *dev;
  dev = devfs_createdev("time");
  if (dev!=NULL) {
    devfs_onread(dev,time_read);
    devfs_onwrite(dev,time_write);
    return 0;
  }
  else return -1;
}
