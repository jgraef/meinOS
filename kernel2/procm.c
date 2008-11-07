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
#include <elf.h>

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
  if (syscall_create(SYSCALL_PROC_GETNAME,proc_getname,3)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETPIDBYNAME,proc_getpidbyname,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_GETVAR,proc_getvar,0)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_EXIT,proc_exit,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_ABORT,proc_abort,0)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_STOP,proc_stop,0)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_FORK,proc_fork_syscall,1)==-1) return -1;
  if (syscall_create(SYSCALL_PROC_EXEC,proc_exec,5)==-1) return -1;
  return 0;
}

/**
 * Creates a new process
 *  @param name Process name
 *  @return Process
 */
proc_t *proc_create(char *name,uid_t uid,gid_t gid,proc_t *parent,int running) {
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
  new->registers.efl = 0x202;
  new->registers.cs = IDX2SEL(3,PRIV_USER);
  new->registers.ds = IDX2SEL(4,PRIV_USER);
  new->registers.es = IDX2SEL(4,PRIV_USER);
  new->registers.fs = IDX2SEL(4,PRIV_USER);
  new->registers.gs = IDX2SEL(4,PRIV_USER);
  new->registers.ss = IDX2SEL(4,PRIV_USER);
  new->addrspace = memuser_create_addrspace(new);
  new->registers.esp = (uint32_t)memuser_create_stack(new->addrspace);
  //memset(&(new->irq_handler),0,sizeof(new->irq_handler));
  new->time_handler = llist_create();
  new->nice = 0;
  new->ticks_rem = NICE2TICKS(new->nice);
  //new->ipc_objects = llist_create();
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
 *  @return private variable
 */
int proc_getvar() {
  return proc_current->var;
}

/**
 * Exits process (Syscall)
 *  @param ret Return value
 */
void proc_exit(int ret) {
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
  *((int*)(*esp)) = val;
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
 * Forks a process (share virtual memory)
 *  @param proc Process to be forked
 *  @return New process
 */
proc_t *proc_vfork(proc_t *proc) {
  proc_t *new = malloc(sizeof(proc_t));
  memcpy(new,proc,sizeof(proc_t));
  new->pid = proc_nextpid++;
  new->parent = proc;
  new->name = strdup(proc->name);
  llist_push(proc->children,new);
  new->time_handler = llist_copy(proc->time_handler);
  new->ticks_rem = NICE2TICKS(new->nice);
  llist_push(proc_all,new);
  llist_push(proc_running,new);
  return new;
}

/**
 * Forks a process
 *  @param proc Process to be forked
 *  @return New process
 */
proc_t *proc_fork(proc_t *proc) {
  proc_t *new = proc_vfork(proc);
  new->addrspace = memuser_clone_addrspace(new,proc->addrspace);
  return new;
}

/**
 * Forks a process
 *  @param start Entrypoint for new process
 *  @return PID of new process
 */
pid_t proc_fork_syscall(void *start) {
  proc_t *new = proc_fork(proc_current);
  new->registers.eip = (uint32_t)start;
  new->registers.eax = 0;
  return new->pid;
}

/**
 * Executes a new process
 *  @param elf_buf ELF file
 *  @param elf_size Size of ELF file
 *  @param var New var
 *  @param do_fork Whether to fork
 *  @return Success?
 */
int proc_exec(char *_name,void *elf_buf,size_t elf_size,int var,int do_fork) {
  proc_t *new;
  void *entrypoint;
  void *elf_map;
  char *name;

  if (_name!=NULL) name = strdup(_name);
  else name = NULL;

  if (((uintptr_t)elf_buf)%PAGE_SIZE!=0) return -1;

  if (do_fork) new = proc_vfork(proc_current);
  else new = proc_current;

  // map ELF file in kernel address space
  // avoids that when addrspace gets destroyed ELF file gets freed
  size_t elf_pages = (elf_size-1)/PAGE_SIZE+1;
  elf_map = memkernel_findvirt(elf_pages);
  size_t i;

  for (i=0;i<elf_size;i+=PAGE_SIZE) {
    paging_map(elf_map+i,paging_getphysaddr(elf_buf+i),0,0);
    paging_unmap(elf_buf+i);
    memuser_free(new->addrspace,elf_buf+i);
    memuser_syncpds(elf_map+i);
  }

  if (!do_fork) memuser_destroy_addrspace(new->addrspace);
memuser_debug++;
  new->addrspace = memuser_create_addrspace(new);
  entrypoint = elf_load(new->addrspace,elf_map,elf_size);
kprintf("HELLO\n"); while (1);
memuser_debug--;
  if (entrypoint==NULL) panic("Failed loading ELF\n");
  new->registers.eip = (uint32_t)entrypoint;
  new->registers.esp = (uint32_t)memuser_create_stack(new->addrspace);
  new->var = var;
  if (name!=NULL) {
    free(new->name);
    new->name = name;
  }
  // free ELF file
  //for (i=0;i<elf_size;i+=PAGE_SIZE) memphys_free(paging_unmap(elf_map+i));

  if (!do_fork) *interrupt_curregs.eip = new->registers.eip;
  if (!do_fork) *interrupt_curregs.esp = new->registers.esp;
  memuser_load_addrspace(new->addrspace);

  if (!do_fork) proc_shedule();
  return 0;
}
