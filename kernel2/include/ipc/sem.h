#ifndef _IPC_SEM_H_
#define _IPC_SEM_H_

#include <sys/types.h>
#include <ipc.h>
#include <procm.h>

typedef struct ipc_sem_S ipc_sem_t;

typedef struct {
  ipc_sem_t *semset;
  int val;
  pid_t lopid;
  llist_t waitn;
  llist_t waitz;
} ipc_sem_sem_t;

struct ipc_sem_S {
  ipc_obj_t ipc;
  size_t num;
  ipc_sem_sem_t *sems;
  time_t otime;
  time_t ctime;
};

typedef struct {
  unsigned short num;
  short op;
  short flags;
} ipc_sem_op_t;

typedef struct {
  int val;
  proc_t *proc;
} ipc_sem_sleepn_t;

id_t ipc_sem_lastid;

int ipc_sem_init();
id_t ipc_sem_get(key_t key);
int ipc_sem_create(key_t key,size_t nsems,mode_t mode,time_t time);
int ipc_sem_op(id_t id,ipc_sem_op_t *ops,size_t nops,time_t time);
int ipc_sem_getval(id_t id,size_t num);
int ipc_sem_setval(id_t id,size_t num,int val,time_t time);
pid_t ipc_sem_getpid(id_t id,size_t num);
size_t ipc_sem_getcnt(id_t id,size_t num,int type);
int ipc_sem_getall(id_t id,short *buf);
int ipc_sem_setall(id_t id,short *buf,time_t time);
int ipc_sem_stat(id_t id,uid_t *uid,gid_t *gid,uid_t *cuid,gid_t *cgid,mode_t *mode,size_t *nsems,time_t *otime,time_t *ctime);
int ipc_sem_set(id_t id,uid_t uid,gid_t gid,mode_t mode,time_t time);
int ipc_sem_destroy(id_t id);

#endif
