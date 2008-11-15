/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <string.h>
#include <devfs.h>

ssize_t zero_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  memset(buf,0,count);
  return count;
}

ssize_t zero_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return 0;
}

int zero_init() {
  devfs_dev_t *dev;
  dev = devfs_createdev("zero");
  if (dev!=NULL) {
    devfs_onread(dev,zero_read);
    devfs_onwrite(dev,zero_write);
    return 0;
  }
  else return -1;
}
