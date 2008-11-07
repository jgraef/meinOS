#ifndef _FUSE_OPT_
#define _FUSE_OPT_

#define FUSE_ARGS_INIT(argc, argv) { argc, argv, 0 }

struct fuse_args {
  int argc;
  char **argv;
  int allocated;
};

#endif
