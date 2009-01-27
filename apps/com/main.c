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
#include <stdint.h>
#include <stdlib.h>
#include <devfs.h>
#include <stdio.h>

#include "com.h"

static void quit() {
  size_t i;

  for (i=0;i<4;i++) {
    if (devices[i]!=NULL) {
      devfs_removedev(devices[i]->dev);
      free(devices[i]);
    }
  }
}

static struct com_device *com_dev_find(devfs_dev_t *dev) {
  size_t i;

  for (i=0;i<4;i++) {
    if (dev==devices[i]->dev) return devices[i];
  }
  return NULL;
}

static ssize_t com_onread(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  struct com_device *com_dev = com_dev_find(dev);
  if (dev!=NULL) return com_dev_recv(com_dev,buf,count);
  else return -1;
}

static ssize_t com_onwrite(devfs_dev_t *dev,void *buf,size_t count,off_t offset) {
  struct com_device *com_dev = com_dev_find(dev);
  if (dev!=NULL) return com_dev_send(com_dev,buf,count);
  else return -1;
}

int main() {
  uint16_t io_ports[] = {0x3F8,0x2F8,0x3E8,0x2E8};
  size_t i;

  devfs_init();
  atexit(quit);

  for (i=0;i<4;i++) {
    if (io_ports[i]!=0) {
      char devname[5];
      snprintf(devname,5,"com%d",i);
      devfs_dev_t *dev = devfs_createdev(devname);
      if (dev!=NULL) {
        devfs_onread(dev,com_onread);
        devfs_onwrite(dev,com_onwrite);
        devices[i] = malloc(sizeof(struct com_device));
        devices[i]->base = io_ports[i];
        devices[i]->dev = dev;
      }
      else devices[i] = NULL;
    }
    else devices[i] = NULL;
  }

  devfs_mainloop();

  return 0;
}
