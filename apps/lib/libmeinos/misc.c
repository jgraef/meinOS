#include <sys/types.h>
#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <llist.h>
#include <rpc.h>
#include <stdio.h>
#include <misc.h>

int _irq_init();
void _libmeinos_init() {
  _irq_init();
}

/**
 * Gets name of a process
 *  @param pid PID of process
 *  @return Name of process (can be passed to free())
 */
char *getname(pid_t pid) {
  size_t size = syscall_call(SYSCALL_PROC_GETNAME,3,pid,NULL,0);
  char *buf = malloc(size);
  syscall_call(SYSCALL_PROC_GETNAME,3,pid,buf,size);
  return buf;
}

/**
 * Mounts a filesystem
 *  @param fs FS name
 *  @param mountpoint Mountpoint
 *  @return Success
 */
int vfs_mount(const char *fs,const char *mountpoint,const char *dev,int readonly) {
  char *func;
  asprintf(&func,"%s_mount",fs);
  if (dev==NULL) dev = "";
  int ret = rpc_call(func,0,fs,mountpoint,dev,readonly);
  free(func);
  return ret;
}

/**
 * Unmounts a filesystem
 *  @param fs FS name
 *  @param mountpoint Mountpoint
 *  @return Success
 */
int vfs_unmount(const char *fs,const char *mountpoint) {
  char *func;
  asprintf(&func,"%s_unmount",fs);
  int ret = rpc_call(func,0,fs,mountpoint);
  free(func);
  return ret;
}
