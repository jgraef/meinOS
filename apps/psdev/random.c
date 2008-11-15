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
#include <devfs.h>
#include <stdlib.h>

ssize_t random_read(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  ssize_t count_rem = count;
  while (count_rem>sizeof(int)) {
    *((int*)buf) = rand();
    count_rem -= sizeof(int);
    buf += sizeof(int);
  }
  int rest = rand();
  memcpy(buf,&rest,count_rem);
  return count;
}

ssize_t random_write(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  return 0;
}

int random_init() {
  devfs_dev_t *dev1;
  devfs_dev_t *dev2;

  dev1 = devfs_createdev("random");
  if (dev1==NULL) return -1;
  devfs_onread(dev1,random_read);
  devfs_onwrite(dev1,random_write);

  dev2 = devfs_createdev("urandom");
  if (dev2==NULL) return -1;
  devfs_onread(dev2,random_read);
  devfs_onwrite(dev2,random_write);

  return 0;
}

