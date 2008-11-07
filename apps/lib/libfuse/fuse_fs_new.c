#include <sys/types.h>
#include <fuse.h>
#include <stdlib.h>
#include <unistd.h>

struct fuse_fs *fuse_fs_new(const struct fuse_operations *op,size_t op_size,void *user_data) {
  struct fuse_fs *fs = malloc(sizeof(struct fuse_fs));
  fs->op = op;
  fs->op_size = op_size;
  fs->user_data = user_data;
  return fs;
}
