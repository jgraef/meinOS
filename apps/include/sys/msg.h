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

#ifndef _SYS_MSG_H_
#define _SYS_MSG_H_

#include <sys/types.h>
#include <sys/ipc.h>

#define MSG_NOERROR 0x16000

typedef size_t msgqnum_t;
typedef size_t msglen_t;

struct msqid_ds {
  struct ipc_perm msg_perm;
  msgqnum_t       msg_qnum;
  msglen_t        msg_qbytes;
  pid_t           msg_lspid;
  pid_t           msg_lrpid;
  time_t          msg_stime;
  time_t          msg_rtime;
  time_t          msg_ctime;
};

int msgget(key_t key,int flags);
int msgsnd(int id,const void *msg,size_t size,int flags);
ssize_t msgrcv(int id,void *buf,size_t size,long type,int flags);
int msgctl(int msqid,int cmd,struct msqid_ds *buf);

#endif
