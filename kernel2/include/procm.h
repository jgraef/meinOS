/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _PROCM_H_
#define _PROCM_H_

typedef struct proc_S proc_t;

#include <sys/types.h>
#include <stdint.h>
#include <paging.h>
#include <llist.h>
#include <memuser.h>
#include <ipc.h>
#include <vm86.h>

#define NICE2TICKS(nice) 1 /*(50-((nice)+20))*/

struct proc_registers {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t eip;
  uint32_t efl;
  uint32_t cs;
  uint32_t ds;
  uint32_t es;
  uint32_t fs;
  uint32_t gs;
  uint32_t ss;
};

/// Process structure
/// @todo do check for system instead for root-uid/gid
/// @todo move uid/gid to var
struct proc_S {
  /// Process ID
  pid_t pid;

  /// Owner UID
  uid_t uid,euid,suid;

  /// Owner GID
  gid_t gid,egid,sgid;

  /// If system process
  int system;

  /// Process name
  char *name;

  /// Parent process
  proc_t *parent;

  /// Children
  llist_t children;

  /// Primary registers
  struct proc_registers registers;

  /// Address space
  addrspace_t *addrspace;

  /// Time handlers
  llist_t time_handler;

  /// Nice value
  int nice;

  /// Remaining ticks
  clock_t ticks_rem;

  /// Whether process sleeps
  int is_sleeping;

  /// Private variable
  int var;

  /// If process is defunc
  int defunc;

  /// Return value
  int ret;

  /// Signal handler
  void (*signal)(int);

  /// Is VM8086 process
  int is_vm86;

  /// VM8086 Pagedir
  pd_t vm86_pagedir;

  /// waitpid()
  int wait;
  pid_t wait_pid;
  int *wait_stat;

  /// VM8086 segment registers
  struct vm86_segmentregs vm86_segregs;
};

llist_t proc_all;
llist_t proc_running;
llist_t proc_sleeping;
pid_t proc_nextpid;
proc_t *proc_current;

int proc_init();
proc_t *proc_create(char *name,uid_t uid,gid_t gid,proc_t *parent,int system,int running);
int proc_destroy(proc_t *proc);
proc_t *proc_find(pid_t pid);
int proc_sleep(proc_t *proc);
int proc_wake(proc_t *proc);
void proc_shedule();
pid_t proc_getpid();
pid_t proc_getparent(pid_t pid);
pid_t proc_getchild(pid_t pid,size_t i);
uid_t proc_getuid(int idmask);
void proc_setuid(int idmask,uid_t uid);
gid_t proc_getgid(int idmask);
void proc_setgid(int idmask,gid_t gid);
ssize_t proc_getname(pid_t pid,char *buf,size_t maxlen);
int proc_setname(pid_t pid,const char *name);
pid_t proc_getpidbyname(const char *name);
int proc_getvar(pid_t pid);
void proc_setvar(pid_t pid,int var);
pid_t proc_waitpid(pid_t pid,int *stat_loc,int options);
void proc_exit(int ret);
void proc_abort();
void proc_continue();
void proc_stop();
void proc_call(proc_t *proc,void *func,size_t numparams,...);
void proc_push(proc_t *proc,int val);
int proc_pop(proc_t *proc);
void proc_idle();
pid_t proc_create_syscall(char *name,uid_t uid,gid_t gid,pid_t parent_pid);
int proc_destroy_syscall(pid_t proc_pid);
int proc_memmap(pid_t proc_pid,void *virt,void *phys,int writable,int swappable,int cow);
int proc_memunmap(pid_t proc_pid,void *virt);
int proc_memfree(pid_t proc_pid,void *virt);
void *proc_memget(pid_t proc_pid,void *virt,int *exists,int *writable,int *swappable,int *cow);
size_t proc_mempagelist(pid_t proc_pid,void **list,size_t n);
int proc_memalloc(pid_t proc_pid,void *virt,int writable,int swappable);
int proc_system(pid_t proc_pid,int system);
int proc_jump(pid_t proc_pid,void *dest);
int *proc_createstack(pid_t proc_pid);

#endif
