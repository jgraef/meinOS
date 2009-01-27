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

#ifndef _PROC_H_
#define _PROC_H_

#include <sys/types.h>
#include <syscall.h>
#include <signal.h>
#include <unistd.h>

#define getchild(i) getchildbypid(getpid(),i)

static __inline__ pid_t proc_create(char *name,uid_t uid,gid_t gid,pid_t parent) {
  return syscall_call(SYSCALL_PROC_CREATE,4,name,uid,gid,parent);
}

static __inline__ int proc_destroy(pid_t pid) {
  return syscall_call(SYSCALL_PROC_DESTROY,1,pid);
}

static __inline__ int proc_getvar(pid_t pid,int var) {
  return syscall_call(SYSCALL_PROC_GETVAR,1,pid);
}

static __inline__ void proc_setvar(pid_t pid,int var) {
  syscall_call(SYSCALL_PROC_SETVAR,2,pid,var);
}

static __inline__ void proc_setname(pid_t pid,const char *name) {
  syscall_call(SYSCALL_PROC_SETNAME,2,pid,name);
}

static __inline__ void *proc_memmap(pid_t pid,void *virt,void *phys,int writable,int swappable,int cow) {
  return (void*)syscall_call(SYSCALL_PROC_MEMMAP,6,pid,virt,phys,writable,swappable,cow);
}

static __inline__ void *proc_memalloc(pid_t pid,void *virt,int writable,int swappable) {
  return (void*)syscall_call(SYSCALL_PROC_MEMALLOC,4,pid,virt,writable,swappable);
}

static __inline__ int proc_memunmap(pid_t pid,void *virt) {
  return syscall_call(SYSCALL_PROC_MEMUNMAP,2,pid,virt);
}

static __inline__ int proc_memfree(pid_t pid,void *virt) {
  return syscall_call(SYSCALL_PROC_MEMFREE,2,pid,virt);
}

static __inline__ void *proc_memget(pid_t pid,void *virt,int *exists,int *writable,int *swappable,int *cow) {
  return (void*)syscall_call(SYSCALL_PROC_MEMGET,6,pid,virt,exists,writable,swappable,cow);
}

static __inline__ int proc_system(pid_t pid,int system) {
  return syscall_call(SYSCALL_PROC_SYSTEM,2,pid,system);
}

static __inline__ int proc_jump(pid_t pid,void *dest) {
  return syscall_call(SYSCALL_PROC_JUMP,2,pid,dest);
}

static __inline__ int proc_createstack(pid_t pid) {
  return syscall_call(SYSCALL_PROC_CREATESTACK,1,pid);
}

static __inline__ void proc_run(pid_t pid) {
  kill(pid,SIGCONT);
}

static __inline__ void proc_stop(pid_t pid) {
  kill(pid,SIGSTOP);
}

static __inline__ pid_t getpidbyname(const char *name) {
  return syscall_call(SYSCALL_PROC_GETPIDBYNAME,1,name);
}

static __inline__ uid_t getuidbypid(pid_t pid) {
  return syscall_call(SYSCALL_PROC_GETUID,1,pid);
}

static __inline__ uid_t getgidbypid(pid_t pid) {
  return syscall_call(SYSCALL_PROC_GETGID,1,pid);
}

static __inline__ pid_t getppidbypid(pid_t pid) {
  return syscall_call(SYSCALL_PROC_GETPARENT,1,pid);
}

static __inline__ gid_t getpgidbypid(pid_t pid) {
  return syscall_call(SYSCALL_PROC_GETGID,1,pid);
}

static __inline__ pid_t getchildbypid(pid_t pid,size_t i) {
  return syscall_call(SYSCALL_PROC_GETCHILD,2,pid,i);
}

char *getname(pid_t pid);
void **proc_mempagelist(pid_t pid,size_t *_num_pages);
int proc_pack_procdata(char **argv,char **env,char *cwd,mode_t cmask);
int proc_unpack_procdata(int var,int *argc,char ***argv);

#endif
