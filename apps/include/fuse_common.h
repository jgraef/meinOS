#ifndef _FUSE_COMMON_H_
#define _FUSE_COMMON_H_

#include <fuse.h>
#include <fuse_opt.h>
#include <stdint.h>

#define FUSE_MAJOR_VERSION  2
#define FUSE_MINOR_VERSION  8
#define FUSE_CAP_ASYNC_READ (1<<0)

struct fuse_chan {
  int fsid;
};

struct fuse_file_info {
  int flags;
  unsigned long fh_old;
  int writepage;
  unsigned int direct_io:1;
  unsigned int keep_cache:1;
  unsigned int flush:1;
  unsigned int padding:29;
  uint64_t fh;
  uint64_t lock_owner;
};

static inline int fuse_version(void) {
  /// @todo Howto return version?
  return FUSE_MAJOR_VERSION;
}

struct fuse_chan *fuse_mount(const char *mountpoint,struct fuse_args *args);
void fuse_unmount(const char *mountpoint,struct fuse_chan *ch);
int fuse_parse_cmdline(struct fuse_args *args,char **mountpoint,int *multithreaded,int *foreground);
int fuse_daemonize(int foreground);

#endif
