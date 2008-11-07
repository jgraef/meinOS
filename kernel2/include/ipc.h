#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <llist.h>
#include <procm.h>

#define IPC_PRIVATE 0

#define IPC_MSG 2
#define IPC_SEM 3
#define IPC_SHM 4

#define IPC_CREAT  0x201
#define IPC_EXCL   0x202
#define IPC_NOWAIT 0x203

typedef struct {
  int type;
  id_t id;
  key_t key;
  proc_t *owner;
  proc_t *creator;
  mode_t mode;
} ipc_obj_t;

llist_t ipc_objects;
id_t ipc_lastid;

int ipc_init();
void *ipc_find(key_t key,id_t id,int type);

#endif
