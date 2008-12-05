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
#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <llist.h>
#include <rpc.h>
#include <stdio.h>
#include <misc.h>

int _irq_init();
void _libmeinos_init() {
  _irq_init();
}

/**
 * Mounts a filesystem
 *  @param fs FS name
 *  @param mountpoint Mountpoint
 *  @return Success
 */
int vfs_mount(const char *fs,const char *mountpoint,const char *dev,int readonly) {
  char *func;
  asprintf(&func,"%s_mount",fs);
  if (dev==NULL) dev = "";
  int ret = rpc_call(func,0,fs,mountpoint,dev,readonly);
  free(func);
  return ret;
}

/**
 * Unmounts a filesystem
 *  @param fs FS name
 *  @param mountpoint Mountpoint
 *  @return Success
 */
int vfs_unmount(const char *fs,const char *mountpoint) {
  char *func;
  asprintf(&func,"%s_unmount",fs);
  int ret = rpc_call(func,0,fs,mountpoint);
  free(func);
  return ret;
}
