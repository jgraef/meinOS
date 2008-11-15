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
