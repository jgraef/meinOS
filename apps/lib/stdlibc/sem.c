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

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <syscall.h>

#define sem_get(key)                                          syscall_call(SYSCALL_IPC_SEM_GET,1,key)
#define sem_create(key,nsems,flags)                           syscall_call(SYSCALL_IPC_SEM_CREATE,4,key,nsems,flags,time(NULL))
#define sem_op(id,ops,nops)                                   syscall_call(SYSCALL_IPC_SEM_OP,4,id,ops,nops,time(NULL))
#define sem_getval(id,num)                                    syscall_call(SYSCALL_IPC_SEM_GETVAL,2,id,num)
#define sem_setval(id,num,val)                                syscall_call(SYSCALL_IPC_SEM_SETVAL,4,id,num,val,time(NULL))
#define sem_getpid(id,num)                                    syscall_call(SYSCALL_IPC_SEM_GETPID,2,id,num)
#define sem_getcnt(id,num,type)                               syscall_call(SYSCALL_IPC_SEM_GETCNT,3,id,num,type)
#define sem_getall(id,buf)                                    syscall_call(SYSCALL_IPC_SEM_GETALL,2,id,buf)
#define sem_setall(id,buf)                                    syscall_call(SYSCALL_IPC_SEM_SETALL,3,id,buf,time(NULL))
#define sem_stat(id,uid,gid,cuid,cgid,mode,nsems,otime,ctime) syscall_call(SYSCALL_IPC_SEM_STAT,9,id,uid,gid,cuid,cgid,mode,nsems,otime,ctime)
#define sem_set(id,uid,gid,mode)                              syscall_call(SYSCALL_IPC_SEM_SET,5,id,uid,gid,mode,time(NULL))
#define sem_destroy(id)                                       syscall_call(SYSCALL_IPC_SEM_DESTROY,1,id)

int semget(key_t key,int nsems,int flags) {
  int ret;
  if ((flags&IPC_CREAT) || key==IPC_PRIVATE) ret = sem_create(key,nsems,flags&0777);
  if (!(flags&IPC_CREAT) || ret<0) ret = sem_get(key);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

int semop(int id,struct sembuf *ops,size_t nops) {
  ssize_t ret;
  do {
    int sleep = 0;
    ret = sem_op(id,ops,nops);
    while (sleep);
    if (ret<0) {
      errno = ret;
      return -1;
    }
  }
  while (ret<nops);
  errno = 0;
  return 0;
}

int semctl(int id,int num,int cmd,...) {
  va_list args;
  int ret;

  va_start(args,cmd);
  if (cmd==GETVAL) ret = sem_getval(id,num);
  else if (cmd==SETVAL) ret = sem_setval(id,num,va_arg(args,int));
  else if (cmd==GETPID) ret = sem_getpid(id,num);
  else if (cmd==GETNCNT) ret = sem_getcnt(id,num,'n');
  else if (cmd==GETZCNT) ret = sem_getcnt(id,num,'z');
  else if (cmd==GETALL) ret = sem_getall(id,va_arg(args,unsigned short*));
  else if (cmd==SETALL) ret = sem_setall(id,va_arg(args,unsigned short*));
  else if (cmd==IPC_STAT) {
    struct semid_ds *buf = va_arg(args,struct semid_ds*);
    ret = sem_stat(id,&(buf->sem_perm.uid),&(buf->sem_perm.gid),&(buf->sem_perm.cuid),&(buf->sem_perm.cgid),&(buf->sem_perm.mode),&(buf->sem_nsems),&(buf->sem_otime),&(buf->sem_ctime));
  }
  else if (cmd==IPC_SET) {
    struct semid_ds *buf = va_arg(args,struct semid_ds*);
    ret = sem_set(id,buf->sem_perm.uid,buf->sem_perm.gid,buf->sem_perm.mode);
  }
  else if (cmd==IPC_RMID) ret = sem_destroy(id);
  else ret = -EINVAL;
  errno = ret<0?-ret:0;
  return ret<0?-1:0;
}
