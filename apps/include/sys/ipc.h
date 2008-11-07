#ifndef _SYS_IPC_H_
#define _SYS_IPC_H_

#include <sys/types.h>

#define IPC_CREAT  0x200
#define IPC_EXCL   0x400
#define IPC_NOWAIT 0x800

#define IPC_PRIVATE 0

#define IPC_RMID 1
#define IPC_SET  2
#define IPC_STAT 3

struct ipc_perm {
  uid_t uid;   // Owner's UID
  gid_t gid;   // Owner's GID
  uid_t cuid;  // Creator's UID
  gid_t cgid;  // Creator's GID
  mode_t mode; // Permissions
};

key_t ftok(const char *file,int x);

#endif
