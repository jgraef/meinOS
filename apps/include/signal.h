#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <sys/types.h>
#include <syscall.h>

#define SIG_ERR -1

// Default signal handler
#define SIG_IGN _signal_dfl_ignore
#define SIG_DFL NULL

// Signals
#define SIGHUP       1
#define SIGINT       2
#define SIGQUIT      3
#define SIGILL       4
#define SIGTRAP      5
#define SIGABRT      6
#define SIGBUS       7
#define SIGFPE       8
#define SIGKILL      9
#define SIGUSR1     10
#define SIGSEGV     11
#define SIGUSR2     12
#define SIGPIPE     13
#define SIGALRM     14
#define SIGTERM     15
#define SIGSTKFLT   16
#define SIGCHLD     17
#define SIGCONT     18
#define SIGSTOP     19
#define SIGTSTP     20
#define SIGTTIN     21
#define SIGTTOU     22
#define SIGURG      23
#define SIGXCPU     24
#define SIGXFSZ     25
#define SIGVTALRM   26
#define SIGPROF     27
#define SIGWINCH    28
#define SIGIO       29
#define SIGPWR      30
#define SIGSYS      31
#define SIGRTMIN(x) (34+(x))
#define SIGRTMAX(x) (64-(x))

typedef struct {
  int si_signo;
  int si_sigcode;
  int si_errno;
  pid_t si_pid;
  uid_t si_uid;
  void *si_addr;
  int si_status;
  long si_band;
} siginfo_t;

typedef struct {
  size_t num_signals;
  int *sigs;
} sigset_t;

struct sigaction {
  void (*sa_handler)(int sig);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_sigaction)(int sig,siginfo_t *siginfo,void *context);
};

void (*signal(int sig,void (*handler)(int)))(int);
void (*sysv_signal(int sig,void (*handler)(int)))(int);
void _signal_dfl_ignore(int sig);
void _signal_handler(int sig);

static inline int kill(pid_t pid,int sig) {
  return syscall_call(SYSCALL_SIG_SEND,2,pid,sig);
}

static inline int raise(int sig) {
  _signal_handler(sig);
  //kill(getpid(),sig);
  return 0;
}

#endif
