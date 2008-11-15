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

#include <sys/types.h>
#include <limits.h>
#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdlib.h>
#include <unistd.h>
#include <llist.h>
#include <misc.h>

struct fuse *fuse_new(struct fuse_chan *chan,struct fuse_args *args,const struct fuse_operations *op,size_t op_size,void *user_data) {
  struct fuse *fuse = malloc(sizeof(struct fuse));
  fuse->args = args;
  fuse->fs = fuse_fs_new(op,op_size,user_data);
  fuse->name = getname(getpid());
  fuse->exited = 0;
  fuse->files = llist_create();
  fuse->chan = chan;
  fuse->context = malloc(sizeof(struct fuse_context));
  fuse->conn = malloc(sizeof(struct fuse_conn_info));
  memset(fuse->conn,0,sizeof(struct fuse_conn_info));
  fuse->conn->max_write = PAGE_SIZE;
  fuse->conn->max_readahead = PAGE_SIZE;
  _fuse_add(fuse);
  return fuse;
}

