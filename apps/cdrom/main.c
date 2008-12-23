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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <devfs.h>
#include <llist.h>
#include <unistd.h>
#include <rpc.h>

#include "device.h"

static llist_t cdrom_devices;

static ssize_t cdrom_devfs_read(devfs_dev_t *devfs,void *buf,size_t count,off_t offset) {
  struct cdrom_device *dev = devfs->user_data;

  size_t lba = offset/dev->block_size;
  size_t off = offset%dev->block_size;
  size_t count_rem = count;

  while (count_rem>0) {
    size_t count_cur = off+count_rem>dev->block_size?dev->block_size-off:count_rem;
    void *cdrom_buf = cdrom_read(dev,lba,1);
    memcpy(buf,cdrom_buf+off,count_cur);
    off = 0;
    lba++;
    count_rem -= count_cur;
    buf += count_cur;
  }

  return count;
}

static int cdrom_device_init(char *name) {
  struct cdrom_device *dev = cdrom_device_create(name);
  if (dev!=NULL) {
    if (cdrom_inquiry(dev)==0) {
      if (cdrom_start(dev,1,1)==0) {
        if (cdrom_read_capacity(dev)==0) {
          char *devname;
          asprintf(&devname,"cdrom%d",llist_size(cdrom_devices));
          dev->devfs = devfs_createdev(devname);
          free(devname);
          if (dev->devfs!=NULL) {
            devfs_onread(dev->devfs,cdrom_devfs_read);
            dev->devfs->user_data = dev;
            llist_push(cdrom_devices,dev);
            return 0;
          }
        }
      }
    }
    else {
      cdrom_device_destroy(dev);
      return 0;
    }
  }
  cdrom_device_destroy(dev);
  return -1;
}

static int cdrom_init() {
  if (cdrom_buf_init()==-1) return -1;
  devfs_init();
  cdrom_devices = llist_create();

  llist_t list = rpc_list();
  char *name;
  while ((name = llist_pop(list))) {
    if (strncmp(name,"scsi_request_atapi",18)==0) {
      char *dev = name+13; // dev = "atapiXX"
      if (cdrom_device_init(dev)==-1) fprintf(stderr,"Could not initialize ATAPI device %s\n",dev);
    }
    free(name);
  }
  llist_destroy(list);

  return 0;
}

static int cdrom_destroy() {
  struct cdrom_device *dev;
  while ((dev = llist_pop(cdrom_devices))) {
    devfs_removedev(dev->devfs);
    cdrom_device_destroy(dev);
  }
  llist_destroy(cdrom_devices);
  return 0;
}

int main(int argc,char *argv[]) {
  cdrom_init();
  devfs_mainloop();
  cdrom_destroy();
  return 0;
}
