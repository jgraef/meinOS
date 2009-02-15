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

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <sys/types.h>
#include <syscall.h>
#include <string.h>

#define SIG_ERR NULL

// Default signal handler
#define SIG_IGN _signal_dfl_ignore
#define SIG_DFL NULL

#define SIG_BLOCK   1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

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
#define SIG_MAXNUM 64

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
  char sigs[SIG_MAXNUM/sizeof(char)];
} sigset_t;

struct sigaction {
  void (*sa_handler)(int sig);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_sigaction)(int sig,siginfo_t *siginfo,void *context);
};

typedef int sig_atomic_t;

void (*signal(int sig,void (*handler)(int)))(int);
void (*sysv_signal(int sig,void (*handler)(int)))(int);
void _signal_dfl_ignore(int sig);
void _signal_handler(int sig);
int sigaddset(sigset_t *set,int sig);
int sigdelset(sigset_t *set,int sig);
int sigprocmask(int how,const sigset_t *set,sigset_t *oset);

static __inline__ int kill(pid_t pid,int sig) {
  return syscall_call(SYSCALL_SIG_SEND,2,pid,sig);
}

static __inline__ int raise(int sig) {
  _signal_handler(sig);
  //kill(getpid(),sig);
  return 0;
}

static __inline__ int sigemptyset(sigset_t *set) {
  memset(set->sigs,0,SIG_MAXNUM/sizeof(char));
  return 0;
}

static __inline__ int sigfillset(sigset_t *set) {
  memset(set->sigs,1,SIG_MAXNUM/sizeof(char));
  return 0;
}

#endif
