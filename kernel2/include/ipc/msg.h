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

#ifndef _SYS_MSG_H_
#define _SYS_MSG_H_

#include <sys/types.h>
#include <ipc.h>

typedef struct {
  ipc_obj_t ipc;
  llist_t msgs;
  llist_t waiting;

  mode_t mode;
  pid_t lrpid;
  pid_t lspid;
  time_t stime;
  time_t rtime;
  time_t ctime;
} ipc_msg_t;

typedef struct {
  ipc_msg_t *msg;
  void *data;
  size_t size;
  long type;
  int flags;
  proc_t *sender;
} ipc_msg_msg_t;

id_t ipc_msg_lastid;

int ipc_msg_init();
id_t ipc_msg_get(key_t key);
id_t ipc_msg_create(key_t key,mode_t mode,time_t time);
int ipc_msg_destroy(id_t id);
int ipc_msg_send(id_t id,void *data,size_t msgsz,long type,int flags,time_t time);
ssize_t ipc_msg_recv(id_t id,void *data,size_t msgsz,long type,int flags,time_t time);
int msg_stat(id_t id,uid_t *uid,gid_t *gid,uid_t *cuid,gid_t *cgid,mode_t *mode,size_t *num,pid_t *lspid,pid_t *lrpid,time_t *stime,time_t *rtime,time_t *ctime);
int msg_set(id_t id,uid_t uid,gid_t gid,mode_t mode,time_t time);

#endif
