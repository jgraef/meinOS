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
