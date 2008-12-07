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

#include <sys/types.h>
#include <stdint.h>
#include <memkernel.h>
#include <procm.h>
#include <llist.h>
#include <string.h>
#include <memuser.h>
#include <memphys.h>
#include <malloc.h>
#include <kprint.h>
#include <gdt.h>
#include <cpu.h>
#include <syscall.h>
#include <interrupt.h>

/**
 * Initializes process management
 *  @return -1=Success; 0=Failure
 */
int proc_init() {
  proc_all = llist_create();
  proc_running = llist_create();
  proc_sleeping = llist_create();
  proc_nextpid = 1;
  proc_current = NULL;
  if (syscall_create(SYSCALL_PROC_GETPID,proc_getpid,0)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETUID,proc_getuid,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETGID,proc_getgid,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_SETUID,proc_setuid,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_SETGID,proc_setgid,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETPARENT,proc_getparent,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETCHILD,proc_getchild,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETNAME,proc_getname,3)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_SETNAME,proc_setname,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETPIDBYNAME,proc_getpidbyname,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETVAR,proc_getvar,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_SETVAR,proc_setvar,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_EXIT,proc_exit,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_ABORT,proc_abort,0)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_STOP,proc_stop,0)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_CREATE,proc_create_syscall,4)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_DESTROY,proc_destroy_syscall,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_MEMMAP,proc_memmap,6)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_MEMALLOC,proc_memalloc,4)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_MEMUNMAP,proc_memunmap,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_MEMFREE,proc_memfree,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_MEMGET,proc_memget,6)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_SYSTEM,proc_system,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_JUMP,proc_jump,2)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_CREATESTACK,proc_createstack,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_WAITPID,proc_waitpid,3)==-1) return -1;
  return 0;
}

/**
 * Creates a new process
 *  @param name Process name
 *  @return Process
 */
proc_t *proc_create(char *name,uid_t uid,gid_t gid,proc_t *parent,int running,int system) {
  proc_t *new = malloc(sizeof(proc_t));

  new->pid = proc_nextpid++;
  new->uid = uid;
  new->euid = uid;
  new->suid = uid;
  new->gid = gid;
  new->egid = gid;
  new->sgid = gid;
  new->name = strdup(name);
  new->system = system;
  new->parent = parent;
  if (parent!=NULL) llist_push(parent->children,new);
  new->children = llist_create();
  memset(&(new->registers),0,sizeof(new->registers));
  new->registers.efl = 0x202;
  new->registers.cs = IDX2SEL(3,PRIV_USER);
  new->registers.ds = IDX2SEL(4,PRIV_USER);
  new->registers.es = IDX2SEL(4,PRIV_USER);
  new->registers.fs = IDX2SEL(4,PRIV_USER);
  new->registers.gs = IDX2SEL(4,PRIV_USER);
  new->registers.ss = IDX2SEL(4,PRIV_USER);
  new->addrspace = memuser_create_addrspace(new);
  new->time_handler = llist_create();
  new->nice = 0;
  new->ticks_rem = NICE2TICKS(new->nice);
  new->var = -1;
  new->defunc = 0;
  new->is_sleeping = !running;
  new->signal = NULL;
  new->is_vm86 = 0;

  llist_push(proc_all,new);
  llist_push(running?proc_running:proc_sleeping,new);
  return new;
}

/**
 * Destroys a process
 *  @param proc Process
 *  @return Success?
 */
int proc_destroy(proc_t *proc) {
  proc_t *child;

  free(proc->name);
  while ((child = llist_pop(proc->children))) child->parent = proc->parent;
  llist_destroy(proc->children);
  if (proc->parent!=NULL) llist_remove(proc->parent->children,llist_find(proc->parent->children,proc));
  if (proc->addrspace!=NULL) memuser_destroy_addrspace(proc->addrspace);
  llist_destroy(proc->time_handler);

  llist_remove(proc_all,llist_find(proc_all,proc));
  if (llist_remove(proc_running,llist_find(proc_running,proc))!=proc) llist_remove(proc_sleeping,llist_find(proc_sleeping,proc));
  free(proc);
  return 0;
}

/**
 * Finds a process by PID
 *  @param PID
 *  @return Process
 */
proc_t *proc_find(pid_t pid) {
  size_t i;
  proc_t *proc;
  if (proc_current->pid==pid) return proc_current;
  for (i=0;(proc = llist_get(proc_all,i));i++) {
    if (proc->pid==pid) return proc;
  }
  return NULL;
}

/**
 * Checks if proc is a child of parent
 *  @param parent Parent process
 *  @param proc Process to check if it's a child of proc
 *  @return If proc is a child of parent
 */
int proc_ischild(proc_t *parent,proc_t *proc) {
  proc_t *child;
  size_t i;

  for (i=0;(child = llist_get(parent->children,i));i++) {
    if (proc==child) return 1;
  }
  return 0;
}

/**
 * Sends a process sleeping
 *  @param proc Process
 *  @param sleep Reference to sleep variable
 *  @return Success?
 */
int proc_sleep(proc_t *proc) {
  if (!proc->is_sleeping) {
    llist_remove(proc_running,llist_find(proc_running,proc));
    llist_push(proc_sleeping,proc);
    proc->is_sleeping = 1;
    if (proc==proc_current) proc_shedule();
  }
  return 0;
}

/**
 * Wakes a process
 *  @param proc Process
 *  @return Success?
 */
int proc_wake(proc_t *proc) {
  if (proc->is_sleeping) {
    llist_remove(proc_sleeping,llist_find(proc_sleeping,proc));
    llist_push(proc_running,proc);
    proc->is_sleeping = 0;
  }
  return 0;
}

/**
 * Save registers of process
 *  @param proc Process
 *  @return Success?
 */
int proc_regs_save(proc_t *proc) {
  proc->registers.eax = *interrupt_curregs.eax;
  proc->registers.ebx = *interrupt_curregs.ebx;
  proc->registers.ecx = *interrupt_curregs.ecx;
  proc->registers.edx = *interrupt_curregs.edx;
  proc->registers.esi = *interrupt_curregs.esi;
  proc->registers.edi = *interrupt_curregs.edi;
  proc->registers.ebp = *interrupt_curregs.ebp;
  proc->registers.esp = *interrupt_curregs.esp;
  proc->registers.eip = *interrupt_curregs.eip;
  proc->registers.efl = *interrupt_curregs.efl;
  proc->registers.cs = *interrupt_curregs.cs;
  proc->registers.ds = *interrupt_curregs.ds;
  proc->registers.es = *interrupt_curregs.es;
  proc->registers.fs = *interrupt_curregs.fs;
  proc->registers.gs = *interrupt_curregs.gs;
  proc->registers.ss = *interrupt_curregs.ss;
  if (proc->is_vm86) vm86_save_segregs(proc);
  return 0;
}

/**
 * Load registers of process
 *  @param proc Process
 *  @return Success?
 */
int proc_regs_load(proc_t *proc) {
  *interrupt_curregs.eax = proc->registers.eax;
  *interrupt_curregs.ebx = proc->registers.ebx;
  *interrupt_curregs.ecx = proc->registers.ecx;
  *interrupt_curregs.edx = proc->registers.edx;
  *interrupt_curregs.esi = proc->registers.esi;
  *interrupt_curregs.edi = proc->registers.edi;
  *interrupt_curregs.ebp = proc->registers.ebp;
  *interrupt_curregs.esp = proc->registers.esp;
  *interrupt_curregs.eip = proc->registers.eip;
  *interrupt_curregs.efl = proc->registers.efl;
  *interrupt_curregs.cs = proc->registers.cs;
  *interrupt_curregs.ds = proc->registers.ds;
  *interrupt_curregs.es = proc->registers.es;
  *interrupt_curregs.fs = proc->registers.fs;
  *interrupt_curregs.gs = proc->registers.gs;
  *interrupt_curregs.ss = proc->registers.ss;
  if (proc->is_vm86) vm86_load_segregs(proc);
  return 0;
}

/**
 * Loads next process for execution
 */
void proc_shedule() {
  size_t i;
  proc_t *proc;
  proc_t *proc_old = proc_current;

  // if no processes running hold machine (until next interrupt)
  if (llist_empty(proc_running)) {
    // if no processes at all, shutdown
    if (llist_empty(proc_sleeping)) cpu_shutdown();
    else {
      /// @todo maybe put these 2 lines in proc_idle
      proc_regs_save(proc_current);
      proc_current = NULL;
      proc_idle();
    }
  }

  // Process finished its time slice
  if (proc_current!=NULL) {
    proc_current->ticks_rem--;
    proc_current = NULL;
  }

  // Search for process that still has time
  for (i=0;(proc = llist_get(proc_running,i));i++) {
    if (proc->ticks_rem>0) proc_current = proc;
  }

  // If no processes with time, fill time slices
  if (proc_current==NULL) {
    //kprintf("Refilling time slices\n");
    for (i=0;(proc = llist_get(proc_running,i));i++) proc->ticks_rem = NICE2TICKS(proc->nice);
    proc_current = llist_get(proc_running,0);
  }

  // Set context and load address space
  if (proc_old!=proc_current) {
    if (proc_old!=NULL) proc_regs_save(proc_old);
    proc_regs_load(proc_current);
    memuser_load_addrspace(proc_current->addrspace);
  }
}

/**
 * Gets PID (Syscall)
 *  @return PID
 */
pid_t proc_getpid() {
  return proc_current->pid;
}

/**
 * Gets Parent PID (Syscall)
 *  @param pid Process to get parent's PID of
 *  @return Parent's PID
 */
pid_t proc_getparent(pid_t pid) {
  proc_t *proc = proc_find(pid);
  if (proc!=NULL) {
    if (proc->parent!=NULL) return proc->parent->pid;
  }
  return 0;
}

/**
 * Gets PID of a child (Syscall)
 *  @param pid Process to get child's PID of
 *  @param i Number of child
 *  @return Child's PID
 */
pid_t proc_getchild(pid_t pid,size_t i) {
  proc_t *proc = proc_find(pid);
  if (proc!=NULL) {
    if (proc->parent!=NULL) {
      proc_t *child = llist_get(proc->parent->children,i);
      if (child==NULL) return -1;
      else return child->pid;
    }
  }
  return 0;
}

/**
 * Gets UID (Syscall)
 *  @param idmask Which ID to return
 *  @return UID
 */
uid_t proc_getuid(int idmask) {
  if ((idmask&1)) return proc_current->uid;
  else if ((idmask&2)) return proc_current->euid;
  else if ((idmask&4)) return proc_current->suid;
  else return 0;
}

/**
 * Sets UID (Syscall)
 *  @param idmask Which ID to set
 *  @param uid New UID
 *  @todo Check permissions
 */
void proc_setuid(int idmask,uid_t uid) {
  if ((idmask&1)) proc_current->uid = uid;
  else if ((idmask&2)) proc_current->euid = uid;
  else if ((idmask&4)) proc_current->suid = uid;
}

/**
 * Gets GID (Syscall)
 *  @param idmask Which ID to return
 *  @return GID
 */
gid_t proc_getgid(int idmask) {
  if ((idmask&1)) return proc_current->gid;
  else if ((idmask&2)) return proc_current->egid;
  else if ((idmask&4)) return proc_current->sgid;
  else return 0;
}

/**
 * Sets GID (Syscall)
 *  @param idmask Which ID to set
 *  @param uid New GID
 *  @todo Check permissions
 */
void proc_setgid(int idmask,gid_t gid) {
  if ((idmask&1)) proc_current->gid = gid;
  else if ((idmask&2)) proc_current->egid = gid;
  else if ((idmask&4)) proc_current->sgid = gid;
}

/**
 * Gets process name (Syscall)
 *  @param pid PID
 *  @param buf Buffer for name
 *  @param maxlen Maximal length of name
 *  @return Success? (if buf==NULL length of name is returned)
 */
ssize_t proc_getname(pid_t pid,char *buf,size_t maxlen) {
  proc_t *proc = proc_find(pid);
  if (proc!=NULL) {
    if (buf!=NULL) {
      strncpy(buf,proc->name,maxlen);
      return 0;
    }
    else return strlen(proc->name)+1;
  }
  else return -1;
}

/**
 * Sets new process name (Syscall)
 *  @param proc_pid PID
 *  @param name New process name
 *  @return Success?
 */
int proc_setname(pid_t proc_pid,const char *name) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL && proc!=proc_current) {
      free(proc->name);
      proc->name = strdup(name);
    }
  }
  return -1;
}

/**
 * Gets PID by process name
 *  @param name Process name
 *  @return PID of process
 */
pid_t proc_getpidbyname(const char *name) {
  size_t i;
  proc_t *proc;
  for (i=0;(proc = llist_get(proc_all,i));i++) {
    if (strcmp(proc->name,name)==0) return proc->pid;
  }
  return -1;
}

/**
 * Gets private variable
 *  @param pid Process' PID
 *  @return private variable
 */
int proc_getvar(pid_t pid) {
  proc_t *proc = proc_find(pid);
  if (proc!=NULL && (proc_current->system || proc_current==proc)) return proc->var;
  else return -1;
}

/**
 * Sets private variable
 *  @param pid Process' PID
 *  @param var New private variable
 */
void proc_setvar(pid_t pid,int var) {
  proc_t *proc = proc_find(pid);
  if (proc!=NULL) proc->var = var;
}

/**
 * Waits for child(ren)
 *  @param pid PID of child to wait for (-1 for all childs)
 *  @param stat_loc Reference for status
 *  @param options Options
 */
pid_t proc_waitpid(pid_t pid,int *stat_loc,int options) {
  if (pid>0) {
    proc_t *proc = proc_find(pid);
    if (proc!=NULL) {
      if (proc->defunc) {
        *stat_loc = proc->ret;
        return pid;
      }
    }
    else return -1;
  }
  proc_current->wait_pid = pid;
  proc_current->wait_stat = stat_loc;
  proc_current->wait = 1;
  proc_sleep(proc_current);
  return -1;
}

/**
 * Exits process (Syscall)
 *  @param ret Return value
 */
void proc_exit(int ret) {
  if (proc_current->parent!=NULL) {
    if (proc_current->parent->wait) {
      pid_t pid = proc_current->parent->wait_pid;
      if (pid==-1 || (pid>0 && pid==proc_current->pid) || (pid==0 && proc_current->gid==proc_current->parent->gid) || (pid<-1 && -pid==proc_current->gid)) {
        proc_current->parent->registers.eax = proc_current->pid;
        if (proc_current->parent->wait_stat!=NULL) *(proc_current->parent->wait_stat) = ret;
        proc_current->parent->wait = 0;
        proc_wake(proc_current->parent);
      }
    }
  }

  proc_current->ret = ret;
  proc_current->defunc = 1;
  llist_remove(proc_running,llist_find(proc_running,proc_current));
  //memuser_destroy_addrspace(proc_current->addrspace);
  //proc_current->addrspace = NULL;
  proc_current = NULL;
  proc_idle();
}

/**
 * Aborts process (Syscall)
 */
void proc_abort() {
  kprintf("Program aborted: %s #%d\n",proc_current->name,proc_current->pid);
  proc_exit(1);
}

/**
 * Stops process (Syscall)
 */
void proc_stop() {
  proc_sleep(proc_current);
}

/**
 * Calls a function in process
 *  @param proc Process
 *  @param func Function
 *  @param numparams Number of parameters
 *  @param ... Parameters
 */
void proc_call(proc_t *proc,void *func,size_t numparams,...) {
  if (!proc->defunc) {
    va_list args;
    size_t i;
    int *params = malloc(numparams*sizeof(int));
    uint32_t *eip = proc_current==proc?interrupt_curregs.eip:&proc->registers.eip;

    va_start(args,numparams);
    for (i=0;i<numparams;i++) params[numparams-(i+1)] = va_arg(args,int);
    va_end(args);

    memuser_load_addrspace(proc->addrspace);
    for (i=0;i<numparams;i++) proc_push(proc,params[i]);
    proc_push(proc,*eip);
    if (proc_current!=NULL) {
      memuser_load_addrspace(proc_current->addrspace);
    }
    *eip = (uint32_t)func;

    //proc_wake(proc);
  }
}

/**
 * Push an element on user stack
 *  @param proc Process
 *  @param val Value
 */
void proc_push(proc_t *proc,int val) {
  uint32_t *esp = proc_current==proc?interrupt_curregs.esp:&proc->registers.esp;
  *esp -= sizeof(int);
  //memuser_load_addrspace(proc->addrspace);
  *((int*)(*esp)) = val;
  //memuser_load_addrspace(proc_current->addrspace);
}

/**
 * Pops an element from user stack
 *  @param proc Process
 *  @return Value
 */
int proc_pop(proc_t *proc) {
  int val = *((int*)proc->registers.esp);
  proc->registers.esp += sizeof(int);
  return val;
}

/**
 * Idles until next schedule
 */
void proc_idle() {
  asm("mov %0,%%esp"::"r"(cpu_this->tss->esp0)); // reload ESP0 in TSS "by hand"
  interrupt_enable(1);
  cpu_halt();
}

/**
 * Creates a process (Syscall)
 *  @param name Name
 *  @param uid UID
 *  @param gid GID
 *  @param parent_pid Parent's PID
 *  @return PID of created process
 */
pid_t proc_create_syscall(char *name,uid_t uid,gid_t gid,pid_t parent_pid) {
  if (proc_current->system) {
    proc_t *parent = proc_find(parent_pid);
    if (parent!=NULL) {
      proc_t *proc = proc_create(name,uid,gid,parent,0,0);
      return proc!=NULL?proc->pid:-1;
    }
  }
  return -1;
}

/**
 * Destroys a process (Syscall)
 *  @param proc_pid PID of process to destroy
 */
int proc_destroy_syscall(pid_t proc_pid) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL && proc!=proc_current) {
      return proc_destroy(proc);
    }
  }
  return -1;
}

/**
 * Maps a page to a process' address space
 *  @param proc_pid Process' PID
 *  @param virt Virtual address where to map
 *  @param phys Address of physical page
 *  @param writable Whether to map it writable
 *  @param cow Map page as COW
 *  @return Success?
 *  @todo remove memuser_load_addrspace()
 */
int proc_memmap(pid_t proc_pid,void *virt,void *phys,int writable,int swappable,int cow) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL) {
      pte_t pte = paging_getpte_pd(virt,proc->addrspace->pagedir);
      if (!pte.exists) {
        memuser_load_addrspace(proc->addrspace);
        paging_map_pd(virt,phys,1,writable && !cow,swappable,cow && writable,proc->addrspace->pagedir);
        memuser_load_addrspace(proc_current->addrspace);
        llist_push(proc->addrspace->pages_loaded,virt);
        return 0;
      }
    }
  }
  return -1;
}

/**
 * Allocates a page for a process
 *  @param proc_pid Process' PID
 *  @param virt Virtual address
 *  @param writable Whether to alloc wirtable memory
 *  @param swappable Whether memory should be swappable
 *  @return Success?
 */
int proc_memalloc(pid_t proc_pid,void *virt,int writable,int swappable) {
  void *page = memphys_alloc();
  if (proc_memmap(proc_pid,virt,page,writable,swappable,0)==0) return 0;
  else {
    memphys_free(page);
    return -1;
  }
}

/**
 * Unmaps a page from a process' address space
 *  @param proc_pid Process' PID
 *  @param virt Virtual address to unmap
 *  @return Success?
 */
int proc_memunmap(pid_t proc_pid,void *virt) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL) {
      memuser_load_addrspace(proc->addrspace);
      paging_unmap(virt);
      memuser_load_addrspace(proc_current->addrspace);
      llist_remove(proc->addrspace->pages_imaginary,llist_find(proc->addrspace->pages_imaginary,virt));
      return 0;
    }
  }
  return -1;
}

/**
 * Frees a page from a process' address space
 *  @param proc_pid Process' PID
 *  @param virt Virtual address to unmap
 *  @return Success?
 */
int proc_memfree(pid_t proc_pid,void *virt) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL) {
      memuser_load_addrspace(proc->addrspace);
      memphys_free(paging_unmap(virt));
      memuser_load_addrspace(proc_current->addrspace);
      llist_remove(proc->addrspace->pages_imaginary,llist_find(proc->addrspace->pages_imaginary,virt));
      return 0;
    }
  }
  return -1;
}

/**
 * Gets information about page of process
 *  @param proc_pid Process' PID
 *  @param virt Virtual address to get information about
 *  @return Success?
 */
void *proc_memget(pid_t proc_pid,void *virt,int *exists,int *writable,int *swappable,int *cow) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL) {
      pte_t pte = paging_getpte_pd(virt,proc->addrspace->pagedir);
      if (exists!=NULL) *exists = pte.exists;
      if (writable!=NULL) *writable = pte.writable;
      if (swappable!=NULL) *swappable = pte.swappable;
      if (cow!=NULL) *cow = pte.cow;
      return pte.in_memory?PAGE2ADDR(pte.page):NULL;
    }
  }
  *exists = 0;
  return NULL;
}

/**
 * Gives/Withdraws a process system privilegs
 *  @param proc_pid Process' PID
 *  @param system Whether process should have system privilegs or not
 *  @return Success?
 */
int proc_system(pid_t proc_pid,int system) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL && (system==1 || proc_current==proc->parent || proc_current==proc)) {
      proc->system = system;
      return 0;
    }
  }
  return -1;
}

/**
 * Jumps a process to a specified destination
 *  @param proc_pid Process' PID
 *  @param dest Destination to jump to
 *  @return Success
 */
int proc_jump(pid_t proc_pid,void *dest) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL) {
      proc->registers.eip = (uint32_t)dest;
      return 0;
    }
  }
  return -1;
}

/**
 * Creates a stack for a process
 *  @param proc_pid Process' PID
 *  @return Stack address
 */
int *proc_createstack(pid_t proc_pid) {
  if (proc_current->system) {
    proc_t *proc = proc_find(proc_pid);
    if (proc!=NULL && proc->addrspace->stack==NULL) {
      int *stack = memuser_create_stack(proc->addrspace);
      proc->registers.esp = (uint32_t)stack;
      return stack;
    }
  }
  return NULL;
}
