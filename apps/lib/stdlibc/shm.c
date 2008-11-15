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
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <syscall.h>
#include <errno.h>

#define shm_get(key)                                                                 syscall_call(SYSCALL_IPC_SHM_GET,1,key)
#define shm_create(key,size,flags)                                                   syscall_call(SYSCALL_IPC_SHM_CREATE,4,key,size,flags,time(NULL))
#define shm_at(id,addr,flags)                                                        syscall_call(SYSCALL_IPC_SHM_ATTACH,4,id,addr,flags,time(NULL))
#define shm_dt(addr)                                                                 syscall_call(SYSCALL_IPC_SHM_DETACH,2,addr,time(NULL))
#define shm_stat(id,uid,gid,cuid,cgid,mode,segsz,lpid,cpid,nattch,atime,dtime,ctime) syscall_call(SYSCALL_IPC_SHM_STAT,13,id,uid,gid,cuid,cgid,mode,segsz,lpid,cpid,nattch,atime,dtime,ctime)
#define shm_set(id,uid,gid,mode)                                                     syscall_call(SYSCALL_IPC_SHM_SET,7,id,uid,gid,mode,time(NULL),5)
#define shm_destroy(id)                                                              syscall_call(SYSCALL_IPC_SHM_DESTROY,1,id)

int shmget(key_t key,size_t size,int flags) {
  int ret;
  if ((flags&IPC_CREAT) || key==IPC_PRIVATE) ret = shm_create(key,size,flags);
  if ((!(flags&IPC_CREAT) || ret<0) && !(flags&IPC_EXCL)) ret = shm_get(key);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

void *shmat(int id,const void *addr,int flags) {
  void *virt = (void*)addr;
  int ret = shm_at(id,&virt,flags);
  errno = ret<0?-ret:0;
  return ret<0?NULL:virt;
}

int shmdt(const void *addr) {
  int ret = shm_dt(addr);
  errno = ret<0?-ret:0;
  return ret<0?-1:0;
}

int shmctl(int id,int cmd,struct shmid_ds *buf) {
  int ret;
  if (cmd==IPC_STAT) ret = shm_stat(id,&(buf->shm_perm.uid),&(buf->shm_perm.gid),&(buf->shm_perm.cuid),&(buf->shm_perm.cgid),&(buf->shm_perm.mode),&(buf->shm_segsz),&(buf->shm_lpid),&(buf->shm_cpid),&(buf->shm_nattch),&(buf->shm_atime),&(buf->shm_dtime),&(buf->shm_ctime));
  else if (cmd==IPC_SET) ret = shm_set(id,buf->shm_perm.uid,buf->shm_perm.gid,buf->shm_perm.mode);
  else if (cmd==IPC_RMID) shm_destroy(id);
  else ret = -EINVAL;
  errno = ret<0?-ret:0;
  return ret<0?-1:0;
}
