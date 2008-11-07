#include <sys/types.h>
#include <fuse.h>

void fuse_teardown(struct fuse *fuse,char *mountpoint) {
  fuse_unmount(mountpoint,fuse->chan);
  fuse_destroy(fuse);
}
