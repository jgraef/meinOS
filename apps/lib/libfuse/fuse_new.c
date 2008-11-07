#include <sys/types.h>
#include <limits.h>
#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdlib.h>
#include <unistd.h>
#include <llist.h>
#include <misc.h>

struct fuse *fuse_new(struct fuse_chan *chan,struct fuse_args *args,const struct fuse_operations *op,size_t op_size,void *user_data) {
  struct fuse *fuse = malloc(sizeof(struct fuse));
  fuse->args = args;
  fuse->fs = fuse_fs_new(op,op_size,user_data);
  fuse->name = getname(getpid());
  fuse->exited = 0;
  fuse->files = llist_create();
  fuse->chan = chan;
  fuse->context = malloc(sizeof(struct fuse_context));
  fuse->conn = malloc(sizeof(struct fuse_conn_info));
  memset(fuse->conn,0,sizeof(struct fuse_conn_info));
  fuse->conn->max_write = PAGE_SIZE;
  fuse->conn->max_readahead = PAGE_SIZE;
  _fuse_add(fuse);
  return fuse;
}

