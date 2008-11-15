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

#include <fuse.h>
#include <stdlib.h>
#include <rpc.h>
#include <unistd.h>
#include <misc.h>

struct fuse_chan *fuse_mount(const char *mountpoint,struct fuse_args *args) {
  char *fsname = getname(getpid());
  int fsid = rpc_call("vfs_regfs",0,fsname,mountpoint);
  free(fsname);
  if (fsid!=-1) {
    struct fuse_chan *chan = malloc(sizeof(struct fuse_chan));
    chan->fsid = fsid;
    return chan;
  }
  else return NULL;
}
