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
