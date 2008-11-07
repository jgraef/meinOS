#include <fuse.h>
#include <string.h>

int fuse_parse_cmdline(struct fuse_args *args,char **mountpoint,int *multithreaded,int *foreground) {
  size_t i;
  *mountpoint = NULL;
  *foreground = 0;
  for (i=1;i<args->argc;i++) {
    if (args->argv[i][0]=='-') { // Switch
      if (strcmp(args->argv[i],"-m")==0) *multithreaded = 1;
      if (strcmp(args->argv[i],"-f")==0) *foreground = 1;
    }
    else *mountpoint = args->argv[i];
  }
  return 0;
}
