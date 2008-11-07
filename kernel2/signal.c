#include <signal.h>
#include <procm.h>
#include <syscall.h>
#include <debug.h>
#include <perm.h>

int signal_init() {
  if (syscall_create(SYSCALL_SIG_SETUP,signal_setup,1)==-1) return -1;
  if (syscall_create(SYSCALL_SIG_SEND,signal_send_syscall,2)==-1) return -1;
  return 0;
}

void signal_setup(void (*handler)(int)) {
  proc_current->signal = handler;
}

void signal_send(proc_t *proc,int sig) {
  if (sig==SIGCONT) proc_wake(proc); // wake process even if it has no signal handler for it
  if (proc->signal!=NULL) proc_call(proc,proc->signal,1,sig);
}

void signal_send_syscall(pid_t pid,int sig) {
  proc_t *proc = proc_find(pid);
  if (proc!=NULL) {
    if (proc->uid==proc_current->uid || proc->gid==proc_current->gid || proc_current->uid==PERM_ROOTUID || proc_current->gid==PERM_ROOTGID) signal_send(proc,sig);
  }
}
