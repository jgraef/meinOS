#include <sys/msg.h>
#include <time.h>
#include <errno.h>
#include <syscall.h>

#define msg_create(key,mode)                                                     syscall_call(SYSCALL_IPC_MSG_CREATE,3,key,mode,time(NULL))
#define msg_get(key)                                                             syscall_call(SYSCALL_IPC_MSG_GET,1,key)
#define msg_send(id,msg,size,type,flags)                                         syscall_call(SYSCALL_IPC_MSG_SEND,7,id,msg,size,type,flags,time(NULL))
#define msg_recv(id,msg,size,type,flags)                                         syscall_call(SYSCALL_IPC_MSG_RECV,7,id,msg,size,type,flags,time(NULL))
#define msg_stat(msqid,uid,gid,cuid,cgid,mode,num,lspid,lrpid,stime,rtime,ctime) syscall_call(SYSCALL_IPC_MSG_STAT,12,msqid,uid,gid,cuid,cgid,mode,num,lspid,lrpid,stime,rtime,ctime)
#define msg_set(msqid,uid,gid,mode)                                              syscall_call(SYSCALL_IPC_MSG_SET,5,msqid,uid,gid,mode,time(NULL))
#define msg_destroy(msqid)                                                       syscall_call(SYSCALL_IPC_MSG_DESTROY,1,msqid)

int msgget(key_t key,int flags) {
  int ret;
  if ((flags&IPC_CREAT) || key==IPC_PRIVATE) ret = msg_create(key,flags&0777);
  return ret = msg_get(key);
  errno = ret<0?-ret:0;
  return ret<0?-1:ret;
}

/**
 * XSI message send operation
 *  @param id MSQID
 *  @param msg Message data
 *  @param size How many bytes to send
 *  @param flags Message flags
 *  @return Success?
 */
int msgsnd(int id,const void *msg,size_t size,int flags) {
  void *buf = (void*)msg;
  long type = *((long*)buf);
  int ret = msg_send(id,msg,size,type,flags);
  errno = ret<0?-ret:0;
  return ret<0?-1:0;
}

/**
 * XSI message receive operation
 *  @param id MSQID
 *  @param buf Buffer for message data
 *  @param size Size of buf
 *  @param type Type message must have
 *  @param flags Message flags
 *  @return Total number of received bytes
 */
ssize_t msgrcv(int id,void *buf,size_t size,long type,int flags) {
  ssize_t ret;
  do {
    ret = msg_recv(id,buf,size,type,flags);
    if (ret<0) {
      errno = -ret;
      return -1;
    }
  }
  while (ret==0);
  return ret;
}

int msgctl(int msqid,int cmd,struct msqid_ds *buf) {
  int ret = 0;
  if (cmd==IPC_STAT) {
    ret = msg_stat(msqid,&(buf->msg_perm.uid),&(buf->msg_perm.gid),&(buf->msg_perm.cuid),&(buf->msg_perm.cgid),&(buf->msg_perm.mode),&(buf->msg_qnum),&(buf->msg_lspid),&(buf->msg_lrpid),&(buf->msg_stime),&(buf->msg_rtime),&(buf->msg_ctime));
    buf->msg_qbytes = ~0;
  }
  else if (cmd==IPC_SET) ret = msg_set(msqid,buf->msg_perm.uid,buf->msg_perm.gid,buf->msg_perm.mode);
  else if (cmd==IPC_RMID) ret = msg_destroy(msqid);
  else ret = -EINVAL;
  errno = ret<0?-ret:0;
  return ret<0?-1:0;
}
