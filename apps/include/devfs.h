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

#ifndef _DEVFS_H_
#define _DEVFS_H_

#include <sys/types.h>
#include <rpc.h>
#include <limits.h>

#define DEVFS_BUFSIZE (2*PAGE_SIZE)

typedef struct devfs_dev_S devfs_dev_t;
struct devfs_dev_S {
  char *name;        // Device name
  dev_t id;          // Device ID
  int shmid;         // SHMID
  void *shmbuf;      // SHM buffer
  ssize_t (*func_read)(devfs_dev_t *dev,void *buf,size_t count,off_t offset);
  ssize_t (*func_write)(devfs_dev_t *dev,void *buf,size_t count,off_t offset);
  void *user_data;   // User data
};

#define devfs_curpid rpc_curpid
#define devfs_mainloop() rpc_mainloop(-1)


/**
 * Sets callback function for fileread
 *  @param dev Device
 *  @param func_read Function to fileread handler
 */
static inline void devfs_onread(devfs_dev_t *dev,ssize_t (*func_read)(devfs_dev_t *dev,void *buf,size_t count,off_t offset)) {
  dev->func_read = func_read;
}

/**
 * Sets callback function for filewrite
 *  @param dev Device
 *  @param func_write Function to filewrite handler
 */
static inline void devfs_onwrite(devfs_dev_t *dev,ssize_t (*func_write)(devfs_dev_t *dev,void *buf,size_t count,off_t offset)) {
  dev->func_write = func_write;
}

void devfs_init();
devfs_dev_t *devfs_createdev(const char *name);
int devfs_removedev(devfs_dev_t *dev);

#endif
