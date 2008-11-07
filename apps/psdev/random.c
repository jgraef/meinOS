#include <sys/types.h>
#include <devfs.h>
#include <stdlib.h>

ssize_t random_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  ssize_t count_rem = count;
  while (count_rem>sizeof(int)) {
    *((int*)buf) = rand();
    count_rem -= sizeof(int);
    buf += sizeof(int);
  }
  int rest = rand();
  memcpy(buf,&rest,count_rem);
  return count;
}

ssize_t random_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return 0;
}

int random_init() {
  devfs_dev_t *dev1;
  devfs_dev_t *dev2;

  dev1 = devfs_createdev("random");
  if (dev1==NULL) return -1;
  devfs_onread(dev1,random_read);
  devfs_onwrite(dev1,random_write);

  dev2 = devfs_createdev("urandom");
  if (dev2==NULL) return -1;
  devfs_onread(dev2,random_read);
  devfs_onwrite(dev2,random_write);

  return 0;
}

