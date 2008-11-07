#ifndef _MISC_H_
#define _MISC_H_

#include <sys/types.h>
#include <syscall.h>
#include <signal.h>

// memory functions
#define mem_getphysaddr(virt) ((void*)syscall_call(SYSCALL_MEM_GETPHYSADDR,1,virt))
#define mem_getvga()          ((void*)syscall_call(SYSCALL_MEM_GETVGA,0))
#define mem_getvgafont()      ((void*)syscall_call(SYSCALL_MEM_GETVGAFONT,0))

// process functions
char *getname(pid_t pid);
#define getuidbypid(pid)      syscall_call(SYSCALL_PROC_GETUID,1,pid)
#define getgidbypid(pid)      syscall_call(SYSCALL_PROC_GETGID,1,pid)
#define getppidbypid(pid)     syscall_call(SYSCALL_PROC_GETPARENT,1,pid)
#define getpgidbypid(pid)     syscall_call(SYSCALL_PROC_GETGID,1,pid)

static inline pid_t getpidbyname(const char *name) {
  return syscall_call(SYSCALL_PROC_GETPIDBYNAME,1,name);
}

// misc functions
#define misc_readbda(off,buf,count) syscall_call(SYSCALL_MISC_READBDA,3,off,buf,count)

// VFS
int vfs_mount(const char *fs,const char *mountpoint,const char *dev,int readonly);
int vfs_unmount(const char *fs,const char *mountpoint);

// Execute
int execute(const char *path,char *argv[]);

// Initialization
static inline void init_ready() {
  kill(1,SIGUSR1);
}

#endif
