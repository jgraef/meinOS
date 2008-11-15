/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
