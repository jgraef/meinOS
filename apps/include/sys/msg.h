#ifndef _SYS_MSG_H_
#define _SYS_MSG_H_

#include <sys/types.h>
#include <sys/ipc.h>

#define MSG_NOERROR 0x16000

typedef size_t msgqnum_t;
typedef size_t msglen_t;

struct msqid_ds {
  struct ipc_perm msg_perm;
  msgqnum_t       msg_qnum;
  msglen_t        msg_qbytes;
  pid_t           msg_lspid;
  pid_t           msg_lrpid;
  time_t          msg_stime;
  time_t          msg_rtime;
  time_t          msg_ctime;
};

int msgget(key_t key,int flags);
int msgsnd(int id,const void *msg,size_t size,int flags);
ssize_t msgrcv(int id,void *buf,size_t size,long type,int flags);
int msgctl(int msqid,int cmd,struct msqid_ds *buf);

#endif
