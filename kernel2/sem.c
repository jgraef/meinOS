#include <sys/types.h>
#include <perm.h>
#include <ipc/sem.h>
#include <ipc.h>
#include <llist.h>
#include <procm.h>
#include <errno.h>
#include <syscall.h>
#include <malloc.h>
#include <debug.h>

#define ipc_sem_permcheck(semset,op) (perm_check((semset)->ipc.owner->pid,(semset)->ipc.owner->gid,proc_current->uid,proc_current->gid,(semset)->ipc.mode,op) || proc_current->uid==(semset)->ipc.creator->uid)
#define ipc_sem_find(key,id) ipc_find(key,id,IPC_SEM)

/**
 * Initializes semaphore IPC
 *  @return Success?
 */
int ipc_sem_init() {
  if (syscall_create(SYSCALL_IPC_SEM_GET,ipc_sem_get,1)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_CREATE,ipc_sem_create,4)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_OP,ipc_sem_op,4)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_GETVAL,ipc_sem_getval,2)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_SETVAL,ipc_sem_setval,4)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_GETPID,ipc_sem_getpid,2)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_GETCNT,ipc_sem_getcnt,3)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_GETALL,ipc_sem_getall,2)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_SETALL,ipc_sem_setall,3)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_STAT,ipc_sem_stat,9)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_SET,ipc_sem_set,5)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_SEM_DESTROY,ipc_sem_destroy,1)==-1) return -1;
  return 0;
}

/**
 * Gets a semaphore set (Syscall)
 *  @param key IPC Key
 *  @return SemID
 */
id_t ipc_sem_get(key_t key) {
  ipc_sem_t *semset = ipc_sem_find(key,-1);
  if (semset==NULL) return -EINVAL;
  else return semset->ipc.id;
}

/**
 * Creates a semaphore set (Syscall)
 *  @param key IPC Key
 *  @param mode Permissions
 *  @param time Current time
 *  @return SemID
 */
int ipc_sem_create(key_t key,size_t nsems,mode_t mode,time_t time) {
  ipc_sem_t *semset = key!=IPC_PRIVATE?ipc_sem_find(key,-1):NULL;
  if (semset==NULL) {
    ipc_sem_t *new = malloc(sizeof(ipc_sem_t));
    if (new!=NULL) {
      size_t i;
      new->ipc.type = IPC_SEM;
      new->ipc.id = ipc_lastid++;
      new->ipc.key = key;
      new->ipc.owner = proc_current;
      new->ipc.creator = proc_current;
      new->ipc.mode = mode;
      new->otime = 0;
      new->ctime = time;
      new->sems = malloc(sizeof(ipc_sem_sem_t)*nsems);
      new->num = nsems;
      for (i=0;i<nsems;i++) {
        new->sems[i].semset = new;
        new->sems[i].val = 0;
        new->sems[i].lopid = 0;
        new->sems[i].waitn = llist_create();
        new->sems[i].waitz = llist_create();
      }
      llist_push(ipc_objects,new);
      return new->ipc.id;
    }
    else return -ENOSPC;
  }
  return -EEXIST;
}

/**
 * Executes semaphore operations (Syscall)
 *  @param id SemID
 *  @param ops Operations
 *  @param nops Number of Operations
 *  @param time Current time
 *  @return Number of operations progressed
 *  @todo Check for permissions
 */
int ipc_sem_op(id_t id,ipc_sem_op_t *ops,size_t nops,time_t time) {
  size_t i;
  for (i=0;i<nops;i++) {
    ipc_sem_t *semset = ipc_sem_find(-1,id);
    if (semset==NULL) return -EINVAL;
    if (ops[i].num>=semset->num) return -1;
    ipc_sem_sem_t *sem = semset->sems+ops[i].num;
    sem->lopid = proc_current->pid;
    semset->otime = time;
    if (ops[i].op<0) {
      if (-ops[i].op<=sem->val) sem->val += ops[i].op;
      else if (-ops[i].op>sem->val && (ops[i].flags&IPC_NOWAIT)) return 0;
      else if (-ops[i].op>sem->val && !(ops[i].flags&IPC_NOWAIT)) {
        ipc_sem_sleepn_t *sleepn = malloc(sizeof(ipc_sem_sleepn_t));
        sleepn->val = -ops[i].op;
        sleepn->proc = proc_current;
        llist_push(sem->waitn,sleepn);
        proc_sleep(proc_current);
        return i;
      }
    }
    else if (ops[i].op>0) sem->val += ops[i].op;
    else if (ops[i].op==0) {
      llist_push(sem->waitz,proc_current);
      proc_sleep(proc_current);
    }
    if (sem->val==0) {
      proc_t *proc;
      while ((proc = llist_pop(sem->waitz))) proc_wake(proc);
    }
    else if (sem->val>0) {
      ipc_sem_sleepn_t *sleepn;
      while ((sleepn = llist_pop(sem->waitn))) {
        if (sleepn->val<sem->val) {
          free(sleepn);
          proc_wake(sleepn->proc);
        }
      }
    }
  }
  return 0;
}

int ipc_sem_getval(id_t id,size_t num) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL && num<semset->num) {
    if (ipc_sem_permcheck(semset,PERM_R)) return semset->sems[num].val;
    else return -EACCES;
  }
  return -EINVAL;
}

int ipc_sem_setval(id_t id,size_t num,int val,time_t time) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL && num<semset->num) {
    if (ipc_sem_permcheck(semset,PERM_W)) {
      semset->sems[num].val = val;
      kprintf("<%d|%d>\n",num,semset->sems[num].val);
      semset->ctime = time;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

pid_t ipc_sem_getpid(id_t id,size_t num) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL && num<semset->num) {
    if (ipc_sem_permcheck(semset,PERM_R)) return semset->sems[num].lopid;
    else return -EACCES;
  }
  return -EINVAL;
}

size_t ipc_sem_getcnt(id_t id,size_t num,int type) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL && num<semset->num) {
    if (ipc_sem_permcheck(semset,PERM_R)) return llist_size(type=='z'?semset->sems[num].waitz:semset->sems[num].waitn);
    else return -EACCES;
  }
  return -EINVAL;
}

int ipc_sem_getall(id_t id,short *buf) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL) {
    if (ipc_sem_permcheck(semset,PERM_R)) {
      size_t i;
      for (i=0;i<semset->num;i++) buf[i] = semset->sems[i].val;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

int ipc_sem_setall(id_t id,short *buf,time_t time) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL) {
    if (ipc_sem_permcheck(semset,PERM_W)) {
      size_t i;
      for (i=0;i<semset->num;i++) semset->sems[i].val = buf[i];
      semset->ctime = time;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

int ipc_sem_stat(id_t id,uid_t *uid,gid_t *gid,uid_t *cuid,gid_t *cgid,mode_t *mode,size_t *nsems,time_t *otime,time_t *ctime) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL) {
    if (ipc_sem_permcheck(semset,PERM_R)) {
      *uid = semset->ipc.owner->uid;
      *gid = semset->ipc.owner->gid;
      *cuid = semset->ipc.creator->uid;
      *cgid = semset->ipc.creator->gid;
      *mode = semset->ipc.mode;
      *nsems = semset->num;
      *otime = semset->otime;
      *ctime = semset->ctime;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

int ipc_sem_set(id_t id,uid_t uid,gid_t gid,mode_t mode,time_t time) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL) {
    if (ipc_sem_permcheck(semset,PERM_W)) {
      semset->ipc.mode = mode;
      semset->ctime = time;
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

/**
 * Destroys a semaphore set (Syscall)
 *  @param id SemID
 *  @return Success?
 */
int ipc_sem_destroy(id_t id) {
  ipc_sem_t *semset = ipc_sem_find(-1,id);
  if (semset!=NULL) {
    if (ipc_sem_permcheck(semset,PERM_W)) {
      size_t i;
      for (i=0;i<semset->num;i++) {
        proc_t *proc;
        ipc_sem_sleepn_t *sleepn;
        while ((proc = llist_pop(semset->sems[i].waitz))) proc_wake(proc);
        while ((sleepn = llist_pop(semset->sems[i].waitn))) {
          if (sleepn->val<semset->sems[i].val) {
            free(sleepn);
            proc_wake(sleepn->proc);
          }
        }
      }
      free(semset->sems);
      free(semset);
      llist_remove(ipc_objects,llist_find(ipc_objects,semset));
    }
    else return -EACCES;
  }
  return -EINVAL;
}
