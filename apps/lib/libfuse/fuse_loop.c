#include <fuse.h>
#include <rpc.h>
#include <misc.h>

int fuse_loop(struct fuse *fuse) {
  init_ready(); /// @todo same as in rpc.c -> rpc_mainloop()
  while (fuse->exited==0) rpc_poll(-1);
  return 0;
}
