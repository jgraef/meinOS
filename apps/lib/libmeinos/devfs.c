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
#include <string.h>
#include <rpc.h>
#include <stdio.h>
#include <sys/shm.h>
#include <llist.h>
#include <errno.h>

#include <devfs.h>

static llist_t devlist;

/**
 * Gets list index by DevID
 *  @param devid DevID
 *  @return List index
 */
static int getlidx_id(int devid) {
  devfs_dev_t *dev;
  int i;
  for (i=0;(dev = llist_get(devlist,i));i++) {
    if (dev->id==devid) return i;
  }
  return -1;
}

/**
 * Reads from device
 *  @param devid DevID
 *  @param data Buffer
 *  @return How many bytes read (-Errorcode)
 */
static int devfs_read(int devid,size_t size,off_t offset) {
  devfs_dev_t *dev = llist_get(devlist,getlidx_id(devid));
  if (dev->func_read!=NULL) {
    if (size>DEVFS_BUFSIZE) size = DEVFS_BUFSIZE;
    return dev->func_read(dev,dev->shmbuf,size,offset);
  }
  else return -ENOSYS;
}

/**
 * Writes to device
 *  @param devid DevID
 *  @param data Buffer
 *  @return How many bytes written (-Errorcode)
 */
static int devfs_write(int devid,size_t size,off_t offset) {
  devfs_dev_t *dev = llist_get(devlist,getlidx_id(devid));
  if (dev->func_write!=NULL) {
    if (size>DEVFS_BUFSIZE) size = DEVFS_BUFSIZE;
    return dev->func_write(dev,dev->shmbuf,size,offset);
  }
  else return -ENOSYS;
}

/**
 * Initializes DevFS
 */
void devfs_init() {
  devlist = llist_create();
  rpc_func(devfs_read,"iii",sizeof(int)*3);
  rpc_func(devfs_write,"iii",sizeof(int)*3);
}

/**
 * Creates an device
 *  @param _name Name of the device
 *  @return Device
 */
devfs_dev_t *devfs_createdev(const char *name) {
  int shmid = shmget(IPC_PRIVATE,DEVFS_BUFSIZE,0);
  if (shmid!=-1) {
    int id = rpc_call("devfs_createdev",0,name,shmid);
    if (id>=0) {
      devfs_dev_t *new = malloc(0*sizeof(devfs_dev_t));
      new->name = strdup(name);
      new->id = id;
      new->shmid = shmid;
      new->shmbuf = shmat(shmid,NULL,0);
      new->func_read = NULL;
      new->func_write = NULL;
      llist_push(devlist,new);
      return new;
    }
    else shmctl(shmid,IPC_RMID,NULL);
  }
  return NULL;
}

/**
 * Removes an device
 *  @param dev Device to remove
 */
int devfs_removedev(devfs_dev_t *dev) {
  int res = rpc_call("devfs_removedev",0,dev->id);
  if (res>=0) {
    llist_remove(devlist,getlidx_id(dev->id));
    shmdt(dev->shmbuf);
    shmctl(dev->shmid,IPC_RMID,NULL);
    free(dev->name);
    free(dev);
  }
  return res;
}
