#include <fuse.h>
#include <stdlib.h>
#include <rpc.h>

void fuse_unmount(const char *mountpoint,struct fuse_chan *chan) {
  rpc_call("vfs_remfs",0,chan->fsid);
  free(chan);
}
