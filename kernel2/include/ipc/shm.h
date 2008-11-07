#ifndef _IPC_SHM_H_
#define _IPC_SHM_H_

#include <sys/types.h>
#include <ipc.h>

#define SHM_RDONLY 0x16000

typedef struct {
  ipc_obj_t ipc;
  size_t size;
  size_t num_pages;
  void **phys;
  llist_t atts;
  pid_t lopid;
  time_t atime;
  time_t dtime;
  time_t ctime;
} ipc_shm_t;

typedef struct {
  ipc_shm_t *shm;
  proc_t *proc;
  void *virt;
  int readonly;
} ipc_shm_att_t;

int ipc_shm_init();
id_t ipc_shm_get(key_t key);
id_t ipc_shm_create(key_t key,size_t size,int flags,time_t time);
int ipc_shm_attach(id_t id,const void **addr,int flags,time_t time);
int ipc_shm_detach(const void *virt,time_t time);
int ipc_shm_stat(id_t id,uid_t *uid,gid_t *gid,uid_t *cuid,gid_t *cgid,mode_t *mode,size_t *segsz,pid_t *lpid,pid_t *cpid,size_t *nattch,time_t *atime,time_t *dtime,time_t *ctime);
int ipc_shm_set(id_t,uid_t uid,gid_t gid,mode_t mode,time_t time);
int ipc_shm_destroy(id_t id);

#endif
