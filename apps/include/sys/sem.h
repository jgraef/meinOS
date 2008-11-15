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

#ifndef _SYS_SEM_H_
#define _SYS_SEM_H_

#include <sys/types.h>
#include <sys/ipc.h>

#define SEM_UNDO 0x16000


#define GETNCNT  4
#define GETPID   5
#define GETVAL   6
#define GETALL   7
#define GETZCNT  8
#define SETVAL   9
#define SETALL  10

struct semid_ds {
  struct ipc_perm sem_perm;
  unsigned short  sem_nsems;
  time_t          sem_otime;
  time_t          sem_ctime;
};

struct sembuf {
  unsigned short sem_num;
  short          sem_op;
  short          sem_flg;
};

int semget(key_t key,int nsems,int flags);
int semop(int id,struct sembuf *ops,size_t nops);
int semctl(int id,int num,int cmd,...);

#endif
