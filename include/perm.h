#ifndef _PERM_H_
#define _PERM_H_

#include <sys/types.h>
#include <llist.h>

#define PERM_R 1
#define PERM_W 2
#define PERM_X 3

#define PERM_ROOTUID 1
#define PERM_ROOTGID 1

#define perm_ingroup(groups,gid) llist_find((groups),(void*)(owner_gid))

int perm_check(uid_t owner_uid,gid_t owner_gid,uid_t access_uid,uid_t access_gid,mode_t mode,int op);

#endif
