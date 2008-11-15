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

#ifndef _SYS_SHM_H_
#define _SYS_SHM_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <limits.h>

#define SHM_RDONLY 0x16000
#define SHM_RND    0x32000

#define SHMLBA PAGE_SIZE

typedef size_t shmatt_t;

struct shmid_ds {
  struct ipc_perm shm_perm;
  size_t          shm_segsz;
  pid_t           shm_lpid;
  pid_t           shm_cpid;
  shmatt_t        shm_nattch;
  time_t          shm_atime;
  time_t          shm_dtime;
  time_t          shm_ctime;
};

int shmget(key_t key,size_t size,int flags);
void *shmat(int id,const void *addr,int flags);
int shmdt(const void *addr);
int shmctl(int id,int cmd,struct shmid_ds *buf);

#endif
