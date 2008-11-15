/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
