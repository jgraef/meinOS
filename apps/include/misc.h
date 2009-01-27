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
#include <rpc.h>
#include <signal.h>

// memory functions
#define mem_getphysaddr(virt) ((void*)syscall_call(SYSCALL_MEM_GETPHYSADDR,1,virt))
#define mem_alloc(size)       ((void*)syscall_call(SYSCALL_MEM_MALLOC,1,size))
#define mem_getvga()          ((void*)syscall_call(SYSCALL_MEM_GETVGA,0))
#define USERSPACE_ADDRESS     0x40000000 /* 1GB */

// VFS
int vfs_mount(const char *fs,const char *mountpoint,const char *dev,int readonly);
int vfs_unmount(const char *fs,const char *mountpoint);

// Initialization
/**
 * Sends init, that initialization has finished
 */
static __inline__ void init_ready() {
  kill(1,SIGUSR1);
}

/**
 * Shuts down computer
 */
static __inline__ void computer_shutdown() {
  rpc_call("computer_shutdown",0);
}

void dbgmsg(const char *fmt,...);

#endif
