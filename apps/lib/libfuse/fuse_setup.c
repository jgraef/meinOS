#include <sys/types.h>
#include <fuse.h>

struct fuse *fuse_setup(int argc,char *argv[],const struct fuse_operations *op,size_t op_size,char **_mountpoint,int multithreaded,void *user_data) {
  struct fuse_args args = FUSE_ARGS_INIT(argc,argv);
  struct fuse *fuse;
  struct fuse_chan *chan;
  int foreground;
  char *mountpoint;

  fuse_parse_cmdline(&args,&mountpoint,NULL,&foreground);
  chan = fuse_mount(mountpoint,&args);
  fuse = fuse_new(chan,&args,op,op_size,user_data);
  fuse->mountpoint = mountpoint;
  fuse_daemonize(foreground);
  if (_mountpoint!=NULL) *_mountpoint = mountpoint;

  return fuse;
}
