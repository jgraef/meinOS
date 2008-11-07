#include <procm.h>
#include <string.h>
#include <gdt.h>
#include <memkernel.h>
#include <llist.h>

proc_t *vm86_proc_create(const char *name,uid_t uid,gid_t gid,proc_t *parent) {
  proc_t *new = malloc(sizeof(proc_t));

  new->pid = proc_nextpid++;
  new->uid = uid;
  new->euid = uid;
  new->suid = uid;
  new->gid = gid;
  new->egid = gid;
  new->sgid = gid;
  new->name = strdup(name);
  new->parent = parent;
  if (parent!=NULL) llist_push(parent->children,new);
  new->children = llist_create();
  memset(&(new->registers),0,sizeof(new->registers));
  new->registers.efl = 0x20202;
  new->registers.cs = IDX2SEL(3,PRIV_USER);
  new->registers.ds = IDX2SEL(4,PRIV_USER);
  new->registers.es = IDX2SEL(4,PRIV_USER);
  new->registers.fs = IDX2SEL(4,PRIV_USER);
  new->registers.gs = IDX2SEL(4,PRIV_USER);
  new->registers.ss = IDX2SEL(4,PRIV_USER);
  new->addrspace = NULL;
  new->vm86_pagedir = NULL;
  new->nice = 0;
  new->ticks_rem = NICE2TICKS(new->nice);
  new->var = -1;
  new->defunc = 0;
  new->is_sleeping = 0;
  new->signal = NULL;
  new->is_vm86 = 1;

  //llist_push(proc_all,new);
  //llist_push(proc_running,new);
  return new;
}

void vm86_save_segregs(proc_t *proc) {
  proc->vm86_segregs.es = *vm86_curregs.es;
  proc->vm86_segregs.ds = *vm86_curregs.ds;
  proc->vm86_segregs.fs = *vm86_curregs.fs;
  proc->vm86_segregs.gs = *vm86_curregs.gs;
}

void vm86_load_segregs(proc_t *proc) {
  *vm86_curregs.es = proc->vm86_segregs.es;
  *vm86_curregs.ds = proc->vm86_segregs.ds;
  *vm86_curregs.fs = proc->vm86_segregs.fs;
  *vm86_curregs.gs = proc->vm86_segregs.gs;
}
