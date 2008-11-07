#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <procm.h>

#define SIGILL       4
#define SIGKILL      9
#define SIGSEGV     11
#define SIGCONT     18
#define SIGSYS      31

#define kill(proc,sig) signal_send(proc,sig)

int signal_init();
void signal_setup(void (*handler)(int));
void signal_send(proc_t *proc,int sig);
void signal_send_syscall(pid_t pid,int sig);

#endif
