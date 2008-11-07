#include <fuse.h>
#include <unistd.h>

int fuse_daemonize(int foreground) {
  if (foreground) return 0;
  else {
    //if (fork()==0) exit(0);
    //return 0;
    return -1;
  }
}
