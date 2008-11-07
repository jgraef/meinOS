#include <sys/types.h>
#include <perm.h>

/**
 * Check permissions
 *
 */
int perm_check(uid_t owner_uid,gid_t owner_gid,uid_t access_uid,uid_t access_gid,mode_t mode,int op) {
  int opmode = 0;
  if (access_uid==PERM_ROOTUID || access_gid==PERM_ROOTGID) return 1;
  if (op==PERM_R) opmode = mode&0444;
  if (op==PERM_W) opmode = mode&0222;
  if (op==PERM_X) opmode = mode&0111;
  if (access_uid==owner_uid && (opmode&0700)) return 1;
  if (access_gid==owner_gid && (opmode&0070)) return 1;
  if ((opmode&0007)) return 1;
  return 0;
}
