#include <sys/types.h>
#include <fuse.h>

int fuse_main_real(int argc,char *argv[],const struct fuse_operations *op,size_t op_size,void *user_data) {
  struct fuse *fuse = fuse_setup(argc,argv,op,op_size,NULL,0,user_data);
  fuse_loop(fuse);
  fuse_teardown(fuse,fuse->mountpoint);
  return 0;
}
