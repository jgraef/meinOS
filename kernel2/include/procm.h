
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
struct proc_S {
  /// Process ID
  pid_t pid;
  /// Owner UID
  uid_t uid,euid,suid;
  /// Owner GID
  gid_t gid,egid,sgid;
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
  /// IRQ handlers
  //void *irq_handler[16];
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
  /// VM8086 segment registers
  struct vm86_segmentregs vm86_segregs;
};

llist_t proc_all;
llist_t proc_running;
llist_t proc_sleeping;
pid_t proc_nextpid;
proc_t *proc_current;

int proc_init();
proc_t *proc_create(char *name,uid_t uid,gid_t gid,proc_t *parent,int running);
int proc_destroy(proc_t *proc);
proc_t *proc_find(pid_t pid);
int proc_sleep(proc_t *proc);
int proc_wake(proc_t *proc);
void proc_shedule();
pid_t proc_getpid();
pid_t proc_getparent(pid_t pid);
uid_t proc_getuid(int idmask);
void proc_setuid(int idmask,uid_t uid);
gid_t proc_getgid(int idmask);
void proc_setgid(int idmask,gid_t gid);
ssize_t proc_getname(pid_t pid,char *buf,size_t maxlen);
pid_t proc_getpidbyname(const char *name);
int proc_getvar();
void proc_exit(int ret);
void proc_abort();
void proc_continue();
void proc_stop();
void proc_call(proc_t *proc,void *func,size_t numparams,...);
void proc_push(proc_t *proc,int val);
int proc_pop(proc_t *proc);
void proc_idle();
proc_t *proc_vfork(proc_t *proc);
proc_t *proc_fork(proc_t *proc);
pid_t proc_fork_syscall(void *start);
int proc_exec(char *name,void *elf_buf,size_t elf_size,int var,int do_fork);

#endif
