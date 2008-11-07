#ifndef _FUSE_LOWLEVEL_H_
#define _FUSE_LOWLEVEL_H_

#include <fuse.h>
#include <llist.h>

#define fuse_chan_destroy(chan)
#define fuse_chan_fd(chan)             (-1)
#define fuse_chan_new(chan)            NULL
#define fuse_chan_recv(chan,buf,size)  (-1)
#define fuse_chan_send(chan,iov,count) (-1)

struct fuse_file {
  struct fuse_file_info *file_info;
  int shmid;
  void *shmbuf;
  char *filename;
  off_t off;
  enum { REG,DIR } type;
  llist_t dir;
};

void _fuse_add(struct fuse *fuse);

#endif
