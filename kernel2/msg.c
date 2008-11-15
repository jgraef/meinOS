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
#include <perm.h>
#include <syscall.h>
#include <llist.h>
#include <string.h>
#include <ipc.h>
#include <ipc/msg.h>
#include <errno.h>
#include <debug.h>

#define ipc_msg_find(key,id) ipc_find(key,id,IPC_MSG)

int ipc_msg_init() {
  if (syscall_create(SYSCALL_IPC_MSG_GET,ipc_msg_get,1)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_MSG_CREATE,ipc_msg_create,3)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_MSG_DESTROY,ipc_msg_destroy,2)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_MSG_SEND,ipc_msg_send,6)==-1) return -1;
  if (syscall_create(SYSCALL_IPC_MSG_RECV,ipc_msg_recv,6)==-1) return -1;
  return 0;
}

/**
 * Gets MSQID by key (Syscall)
 *  @param key IPC Key
 *  @return MSQID
 */
id_t ipc_msg_get(key_t key) {
  ipc_msg_t *msg = ipc_msg_find(key,-1);
  if (msg==NULL) return -1;
  return msg->ipc.id;
}

/**
 * Creates a Message Queue (Syscall)
 *  @param key IPC key
 *  @param flags Flags
 *  @param time Current time
 *  @return MSQID
 */
id_t ipc_msg_create(key_t key,mode_t mode,time_t time) {
  ipc_msg_t *msg = key!=IPC_PRIVATE?ipc_msg_find(key,-1):NULL;
  if (msg==NULL) {
    ipc_msg_t *new = malloc(sizeof(ipc_msg_t));
    if (new!=NULL) {
      memset(new,0,sizeof(ipc_msg_t));
      new->ipc.type = IPC_MSG;
      new->ipc.key = key;
      new->ipc.owner = proc_current;
      new->ipc.creator = proc_current;
      new->ipc.id = ipc_lastid++;
      new->ipc.mode = mode;
      new->msgs = llist_create();
      new->waiting = llist_create();
      new->ctime = time;
      llist_push(ipc_objects,new);
      return new->ipc.id;
    }
  }
  return -EINVAL;
}

/**
 * Destroys a message queue (Syscall)
 *  @param id ID of message queue
 *  @return Success?
 */
int ipc_msg_destroy(id_t id) {
  ipc_msg_t *msq = ipc_msg_find(-1,id);
  if (msq!=NULL) {
    if (perm_check(proc_current->pid,proc_current->gid,msq->ipc.owner->uid,msq->ipc.owner->gid,msq->mode,PERM_W)) {
      ipc_msg_msg_t *msg;
      proc_t *waiter;
      while ((msg = llist_pop(msq->msgs))!=NULL) {
        if ((msg->flags&IPC_NOWAIT)!=msg->flags) proc_wake(msg->sender);
        free(msg->data);
        free(msg);
      }
      while ((waiter = llist_pop(msq->waiting))!=NULL) proc_wake(waiter);
      llist_remove(ipc_objects,llist_find(ipc_objects,msq));
      free(msq);
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

/**
 * Sends a message (Syscall)
 *  @param id MSQID
 *  @param msg Message data
 *  @param msgsz Message size
 *  @param type Message type
 *  @param flags Flags
 *  @param time Current time
 *  @return Success?
 */
int ipc_msg_send(id_t id,void *data,size_t msgsz,long type,int flags,time_t time) {
  proc_t *waiter;
  ipc_msg_t *msq = ipc_msg_find(-1,id);
  if (msq!=NULL) {
    if (perm_check(proc_current->pid,proc_current->gid,msq->ipc.owner->uid,msq->ipc.owner->gid,msq->mode,PERM_W)) {
      ipc_msg_msg_t *new = malloc(sizeof(ipc_msg_msg_t));
      new->msg = msq;
      new->data = memcpy(malloc(msgsz),data,msgsz);
      new->type = type;
      new->size = msgsz;
      new->flags = flags;
      new->sender = proc_current;
      msq->lspid = proc_current->pid;
      msq->stime = time;
      llist_push(msq->msgs,new);
      while ((waiter = llist_pop(msq->waiting))!=NULL) proc_wake(waiter);
      if ((flags&IPC_NOWAIT)!=flags) proc_sleep(proc_current);
      return 0;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

/**
 * Receives a message (Syscall)
 *  @param id MSQID
 *  @param data Buffer for message data
 *  @param msgsz Size of buffer
 *  @param type Type message must have
 *  @param flags Flags
 *  @param time Current time
 *  @return Number of bytes received
 *  @todo Get first message with type, not first absolute message
 */
ssize_t ipc_msg_recv(id_t id,void *data,size_t msgsz,long type,int flags,time_t time) {
  ipc_msg_t *msq = ipc_msg_find(-1,id);
  if (msq!=NULL) {
    if (perm_check(proc_current->pid,proc_current->gid,msq->ipc.owner->uid,msq->ipc.owner->gid,msq->mode,PERM_R)) {
      ipc_msg_msg_t *msg;
      size_t i;
      for (i=0;(msg = llist_get(msq->msgs,i));i++) {
        if (msg->type==type || type==0 || (type<0 && msg->type<=-type)) {
          size_t count = msgsz>msg->size?msg->size:msgsz;
          memcpy(data,msg->data,count);
          if ((msg->flags&IPC_NOWAIT)==flags) proc_wake(msg->sender);
          msq->lrpid = proc_current->pid;
          msq->rtime = time;
          free(msg->data);
          free(msg);
          return count;
        }
      }
      if (!(flags&IPC_NOWAIT)) {
        llist_push(msq->waiting,proc_current);
        proc_sleep(proc_current);
        return 0; // try again after sleeping
      }
      return -ENOMSG;
    }
    else return -EACCES;
  }
  return -EINVAL;
}

/**
 * Gets information about a message queue (Syscall)
 *  @param id MSQID
 *  @param uid Reference for owner's UID
 *  @param gid Reference for owner's GID
 *  @param cuid Reference for creator's UID
 *  @param cgid Reference for creator's GID
 *  @param mode Reference for mode
 *  @param num Reference for number of messages
 *  @param lspid Reference for last sender's PID
 *  @param lrpid Reference for last receiver's PID
 *  @param stime Reference for last send time
 *  @param rtime Reference for last receive time
 *  @param ctime Reference for last change time
 *  @return Success?
 */
int msg_stat(id_t id,uid_t *uid,gid_t *gid,uid_t *cuid,gid_t *cgid,mode_t *mode,size_t *num,pid_t *lspid,pid_t *lrpid,time_t *stime,time_t *rtime,time_t *ctime) {
  ipc_msg_t *msq = ipc_msg_find(-1,id);
  if (msq!=NULL) {
    *uid = msq->ipc.owner->uid;
    *gid = msq->ipc.owner->gid;
    *cuid = msq->ipc.creator->uid;
    *cgid = msq->ipc.creator->gid;
    *mode = msq->mode;
    *num = llist_size(msq->msgs);
    *lspid = msq->lspid;
    *lrpid = msq->lrpid;
    *stime = msq->stime;
    *rtime = msq->rtime;
    *ctime = msq->ctime;
    return 0;
  }
  else return -EINVAL;
}

/**
 * Changes information of a message queue (Syscall)
 *  @param id MSQID
 *  @param uid Owner's UID
 *  @param gid Owner's GID
 *  @param mode Permissions
 *  @param time Current time
 *  @return Success?
 */
int msg_set(id_t id,uid_t uid,gid_t gid,mode_t mode,time_t time) {
  ipc_msg_t *msq = ipc_msg_find(-1,id);
  if (msq!=NULL) {
    if (perm_check(proc_current->pid,proc_current->gid,msq->ipc.owner->uid,msq->ipc.owner->gid,msq->mode,PERM_W) || proc_current==msq->ipc.creator) {
      msq->mode = mode;
      msq->ctime = time;
    }
    return 0;
  }
  return -EINVAL;
}
