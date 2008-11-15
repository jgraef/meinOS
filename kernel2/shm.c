/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <ipc/shm.h>
#include <ipc.h>
#include <memuser.h>
#include <paging.h>
#include <errno.h>
#include <memphys.h>
#include <procm.h>
#include <stdint.h>
#include <malloc.h>
#include <syscall.h>
#include <perm.h>
#include <string.h>
#include <memmap.h>
#include <debug.h>

#define ipc_shm_permcheck(shm,op) (perm_check((shm)->ipc.owner->pid,(shm)->ipc.owner->gid,proc_current->uid,proc_current->gid,(shm)->ipc.mode,op) || proc_current->uid==(shm)->ipc.creator->uid)
#define ipc_shm_find(key,id) ipc_find(key,id,IPC_SHM)

/**
 * Intitializes Shared Memory
 *  @return Success?
 */
int ipc_shm_init() {
  if (syscall_create(SYSCALL_IPC_SHM_GET,ipc_shm_get,1)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SHM_CREATE,ipc_shm_create,4)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SHM_ATTACH,ipc_shm_attach,4)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SHM_DETACH,ipc_shm_detach,2)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SHM_STAT,ipc_shm_stat,13)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SHM_SET,ipc_shm_set,7)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SHM_DESTROY,ipc_shm_destroy,1)==-1) return -1;
  return 0;
}

/**
 * Gets a shared memory object (Syscall)
 *  @param key IPC Key
 *  @return SHMID
 */
id_t ipc_shm_get(key_t key) {
  ipc_shm_t *shm = ipc_shm_find(key,-1);
  if (shm==NULL) return -EINVAL;
  else return shm->ipc.id;
}

/**
 * Creates a shared memory object (Syscall)
 *  @param key IPC Key
 *  @param size Size
 *  @param flags Flags
 *  @param time Current time
 *  @return SHMID
 */
id_t ipc_shm_create(key_t key,size_t size,int flags,time_t time) {
  ipc_shm_t *shm = key!=IPC_PRIVATE?ipc_shm_find(key,-1):NULL;
  if (shm==NULL) {
    ipc_shm_t *new = malloc(sizeof(ipc_shm_t));
    if (new!=NULL) {
      size_t i;
      memset(new,0,sizeof(ipc_shm_t));
      new->ipc.type = IPC_SHM;
      new->ipc.id = ipc_lastid++;
      new->ipc.key = key;
      new->ipc.owner = proc_current;
      new->ipc.creator = proc_current;
      new->ipc.mode = flags&0777;
      new->size = size;
      new->num_pages = ADDR2PAGE(PAGEUP(new->size));
      new->phys = malloc(sizeof(void*)*new->num_pages);
      for (i=0;i<new->num_pages;i++) new->phys[i] = memphys_alloc();
      new->atts = llist_create();
      new->lopid = proc_current->pid;
      llist_push(ipc_objects,new);
      return new->ipc.id;
    }
    else return -ENOSPC;
  }
  else return -EEXIST;
}

/**
 * Attaches process to shared memory object (Syscall)
 *  @param id SHMID
 *  @param virt Virtual address to attatch to
 *  @param flags Flags
 *  @return Success?
 */
int ipc_shm_attach(id_t id,const void **addr,int flags,time_t time) {
  void *virt = (void*)*addr;
  if (virt>=(void*)USERDATA_ADDRESS || virt==NULL) {
    ipc_shm_t *shm = ipc_shm_find(-1,id);
    if (shm!=NULL) {
      ipc_shm_att_t *new = malloc(sizeof(ipc_shm_att_t));
      if (new!=NULL) {
        size_t i;
        new->shm = shm;
        new->proc = proc_current;
        new->virt = virt==NULL?memuser_findvirt(proc_current->addrspace,shm->num_pages):(void*)virt;
        new->readonly = flags&SHM_RDONLY;
        if (virt==NULL) virt = memuser_findvirt(proc_current->addrspace,shm->num_pages);
        for (i=0;i<shm->num_pages;i++) paging_map(new->virt+i*PAGE_SIZE,shm->phys[i],1,!new->readonly);
        llist_push(shm->atts,new);
        shm->atime = time;
        shm->lopid = proc_current->pid;
        *addr = virt;
        return 0;
      }
      else return -ENOSPC;
    }
  }
  return -EINVAL;
}

/**
 * Detaches process form shared memory object (Syscall)
 *  @param virt Virtual address to detach from
 *  @return Success?
 */
int ipc_shm_detach(const void *virt,time_t time) {
  size_t i,j,k;
  ipc_shm_t *shm;
  ipc_shm_att_t *att;
  if (virt<(void*)USERDATA_ADDRESS) return -1;
  for (i=0;(shm = llist_get(ipc_objects,i));i++) {
    if (shm->ipc.type==IPC_SHM) {
      for (j=0;(att = llist_get(shm->atts,j));j++) {
        if (att->proc==proc_current && att->virt==virt) {
          for (k=0;k<shm->num_pages;k++) paging_unmap(att->virt+k*PAGE_SIZE);
          llist_remove(shm->atts,j);
          free(att);
          shm->atime = time;
          shm->lopid = proc_current->pid;
          return 0;
        }
      }
    }
  }
  return -EINVAL;
}

int ipc_shm_stat(id_t id,uid_t *uid,gid_t *gid,uid_t *cuid,gid_t *cgid,mode_t *mode,size_t *segsz,pid_t *lpid,pid_t *cpid,size_t *nattch,time_t *atime,time_t *dtime,time_t *ctime) {
  ipc_shm_t *shm = ipc_shm_find(-1,id);
  if (shm!=NULL) {
    if (ipc_shm_permcheck(shm,PERM_R)) {
      *uid = shm->ipc.owner->uid;
      *gid = shm->ipc.owner->gid;
      *cuid = shm->ipc.creator->uid;
      *cgid = shm->ipc.creator->gid;
      *cpid = shm->ipc.creator->pid;
      *mode = shm->ipc.mode;
      *segsz = shm->size;
      *lpid = shm->lopid;
      *nattch = llist_size(shm->atts);
      *atime = shm->atime;
      *dtime = shm->dtime;
      *ctime = shm->ctime;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

int ipc_shm_set(id_t id,uid_t uid,gid_t gid,mode_t mode,time_t time) {
  ipc_shm_t *shm = ipc_shm_find(-1,id);
  if (shm!=NULL) {
    if (ipc_shm_permcheck(shm,PERM_W)) {
      shm->ipc.mode = mode;
      shm->ctime = time;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

/**
 * Destroys a shared memory object
 *  @param id SHMID
 *  @return Success
 */
int ipc_shm_destroy(id_t id) {
  ipc_shm_t *shm = ipc_shm_find(-1,id);
  if (shm!=NULL) {
    ipc_shm_att_t *att;
    size_t i;
    while ((att = llist_pop(shm->atts))) {
      for (i=0;i<shm->num_pages;i++) paging_unmap(att->virt+i*PAGE_SIZE);
      free(att);
    }
    for (i=0;i<shm->num_pages;i++) memphys_free(shm->phys[i]);
    free(shm->phys);
    free(shm);
    llist_remove(ipc_objects,llist_find(ipc_objects,shm));
    return 0;
  }
  return -1;
}
