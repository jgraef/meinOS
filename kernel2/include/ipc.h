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

#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <llist.h>
#include <procm.h>

#define IPC_PRIVATE 0

#define IPC_MSG 2
#define IPC_SEM 3
#define IPC_SHM 4

#define IPC_CREAT  0x201
#define IPC_EXCL   0x202
#define IPC_NOWAIT 0x203

typedef struct {
  int type;
  id_t id;
  key_t key;
  proc_t *owner;
  proc_t *creator;
  mode_t mode;
} ipc_obj_t;

llist_t ipc_objects;
id_t ipc_lastid;

int ipc_init();
void *ipc_find(key_t key,id_t id,int type);

#endif
