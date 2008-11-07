#include <fuse.h>
#include <stdlib.h>
#include <rpc.h>
#include <unistd.h>
#include <misc.h>

struct fuse_chan *fuse_mount(const char *mountpoint,struct fuse_args *args) {
  char *fsname = getname(getpid());
  int fsid = rpc_call("vfs_regfs",0,fsname,mountpoint);
  free(fsname);
  if (fsid!=-1) {
    struct fuse_chan *chan = malloc(sizeof(struct fuse_chan));
    chan->fsid = fsid;
    return chan;
  }
  else return NULL;
}
