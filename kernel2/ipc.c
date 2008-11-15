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

#include <ipc.h>
#include <llist.h>
#include <ipc/msg.h>
#include <ipc/sem.h>
#include <ipc/shm.h>

/**
 * Initializes IPC
 *  @return Success?
 */
int ipc_init() {
  ipc_objects = llist_create();
  ipc_lastid = 0;
  if (ipc_msg_init()==-1) return -1;
  if (ipc_sem_init()==-1) return -1;
  if (ipc_shm_init()==-1) return -1;
  return 0;
}

/**
 * Finds an IPC object
 *  @param key IPC Key
 *  @param id IPC object's ID
 *  @return IPC object
 */
void *ipc_find(key_t key,id_t id,int type) {
  size_t i;
  ipc_obj_t *ipc;
  for (i=0;(ipc = llist_get(ipc_objects,i));i++) {
    if (ipc->type==type) {
      if ((ipc->key==key || key==-1) && (ipc->id==id || id==-1)) return ipc;
    }
  }
  return NULL;
}
