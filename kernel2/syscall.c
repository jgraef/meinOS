#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <syscall.h>
#include <procm.h>
#include <signal.h>
#include <debug.h>
#include <interrupt.h>

/**
 * Initializes Syscall Management
 *  @return 0=Success; -1=Failure
 */
int syscall_init() {
  memset(&syscalls,0,SYSCALL_MAXNUM*sizeof(struct syscall));
  return 0;
}

/**
 * Syscall handler
 *  @param params Pointer to parameter list
 */
void syscall_handler(uint32_t *stack) {
  proc_t *proc_call = proc_current;
  int res = -1;
  size_t i;
  unsigned int cmd;
  interrupt_save_stack(stack,NULL);
  int *params = ((int*)*interrupt_curregs.esp);

  cmd = *params++;
  if (cmd<=SYSCALL_MAXNUM) {
    if (syscalls[cmd].func!=NULL) {
      for (i=0;i<syscalls[cmd].numparams;i++) asm("push %0"::"a"(params[i]));
      res = syscalls[cmd].func();
    }
    else kill(proc_current,SIGSYS);
  }

  if (proc_current==proc_call) *interrupt_curregs.eax = res;
}

/**
 * Registers a syscall
 *  @param cmd Syscall number
 *  @param func Pointer to function
 *  @param numparams Number of parameters
 *  @return 0=Success; -1=Failure
 */
int syscall_create(int cmd,void *func,int numparams) {
  if (cmd>SYSCALL_MAXNUM || syscalls[cmd].func!=NULL) return -1;
  syscalls[cmd].func = func;
  syscalls[cmd].numparams = numparams;
  return 0;
}

/**
 * Unregisters a syscall
 *  @param cmd Syscall number
 */
void syscall_destroy(int cmd) {
  if (cmd<SYSCALL_MAXNUM) syscalls[cmd].func = NULL;
}
