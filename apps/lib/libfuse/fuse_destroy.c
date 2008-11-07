#include <fuse.h>
#include <stdlib.h>

void fuse_destroy(struct fuse *fuse) {
  if (fuse->fs->op->destroy!=NULL) fuse->fs->op->destroy(fuse->context->private_data);
  free(fuse->name);
  free(fuse->fs);
  free(fuse->context);
  free(fuse->conn);
  free(fuse);
}
