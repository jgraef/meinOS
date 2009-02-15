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

#include <sys/types.h>
#include <syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define SIG_NUM 64

typedef struct {
  void (*dfl)(int);
  void (*handler)(int);
  int masked;
  int sysv;
} signal_t;

static signal_t signals[SIG_NUM];
void _signal_handler_stub(int sig);

static void signal_create(int sig,void (*handler)(int)) {
  signals[sig].dfl = handler;
  signals[sig].handler = handler;
}

void _signal_handler(int sig) {
  if (sig<SIG_NUM) {
    if (signals[sig].handler!=NULL && !signals[sig].masked) {
      signals[sig].handler(sig);
      if (signals[sig].sysv) signals[sig].handler = signals[sig].dfl;
    }
  }
}

void _signal_dfl_ignore(int sig) {
  // just ignore
}

static void signal_dfl_term(int sig) {
  _Exit(EXIT_FAILURE);
}

static void signal_dfl_abort(int sig) {
  abort();
}

static void signal_dfl_cont(int sig) {
  // process continues automatically if it receives SIGCONT
}

static void signal_dfl_stop(int sig) {
  syscall_call(SYSCALL_PROC_STOP,0);
}

void _signal_init() {
  memset(signals,0,sizeof(signals));
  syscall_call(SYSCALL_SIG_SETUP,1,_signal_handler_stub);

  signal_create(SIGABRT,signal_dfl_abort);
  signal_create(SIGALRM,signal_dfl_term);
  signal_create(SIGBUS,signal_dfl_abort);
  signal_create(SIGCHLD,_signal_dfl_ignore);
  signal_create(SIGCONT,signal_dfl_cont);
  signal_create(SIGFPE,signal_dfl_abort);
  signal_create(SIGHUP,signal_dfl_term);
  signal_create(SIGILL,signal_dfl_abort);
  signal_create(SIGINT,signal_dfl_term);
  signal_create(SIGKILL,signal_dfl_term);
  signal_create(SIGPIPE,signal_dfl_term);
  signal_create(SIGQUIT,signal_dfl_abort);
  signal_create(SIGSEGV,signal_dfl_abort);
  signal_create(SIGSTOP,signal_dfl_stop);
  signal_create(SIGTERM,signal_dfl_term);
  signal_create(SIGTSTP,signal_dfl_stop);
  signal_create(SIGTTIN,signal_dfl_stop);
  signal_create(SIGTTOU,signal_dfl_stop);
  signal_create(SIGUSR1,signal_dfl_term);
  signal_create(SIGUSR2,signal_dfl_term);
  signal_create(SIGPROF,signal_dfl_term);
  signal_create(SIGSYS,signal_dfl_abort);
  signal_create(SIGTRAP,signal_dfl_abort);
  signal_create(SIGURG,_signal_dfl_ignore);
  signal_create(SIGXCPU,signal_dfl_abort);
  signal_create(SIGXFSZ,signal_dfl_abort);
}

void (*signal(int sig,void (*handler)(int)))(int) {
  if (sig>0 && sig<SIG_MAXNUM && sig!=SIGKILL && sig!=SIGSTOP) {
    if (handler==SIG_DFL) signals[sig].handler = signals[sig].dfl;
    else signals[sig].handler = handler;
    signals[sig].sysv = 0;
    signals[sig].masked = 0;
  }
  return signals[sig].handler;
}

void (*sysv_signal(int sig,void (*handler)(int)))(int) {
  if (sig>0 && sig<SIG_MAXNUM && sig!=SIGKILL && sig!=SIGSTOP) {
    if (handler==SIG_DFL) signals[sig].handler = signals[sig].dfl;
    else signals[sig].handler = handler;
    signals[sig].sysv = 1;
    signals[sig].masked = 0;
  }
  return signals[sig].handler;
}

int sigaddset(sigset_t *set,int sig) {
  if (sig>0 && sig<SIG_MAXNUM && set!=NULL) {
    sig--;
    set->sigs[sig/sizeof(char)] |= 1<<(sig%sizeof(char));
    return 0;
  }
  else return -1;
}

int sigdelset(sigset_t *set,int sig) {
  if (sig>0 && sig<SIG_MAXNUM && set!=NULL) {
    sig--;
    set->sigs[sig/sizeof(char)] &= ~(1<<(sig%sizeof(char)));
    return 0;
  }
  else return -1;
}

int sigismember(const sigset_t *set,int sig) {
  if (sig>0 && sig<SIG_MAXNUM && set!=NULL) {
    sig--;
    return set->sigs[sig/sizeof(char)]&(1<<(sig%sizeof(char)));
  }
  else return -1;
}

int sigprocmask(int how,const sigset_t *set,sigset_t *oset) {
  int i;

  if (how==SIG_BLOCK && set!=NULL) {
    for (i=0;i<SIG_MAXNUM;i++) {
      if (sigismember(set,i)) signals[i].masked = 1;
      if (oset!=NULL) {
        if (signals[i].masked) sigaddset(oset,i);
        else sigdelset(oset,i);
      }
    }
    return 0;
  }
  else if (how==SIG_UNBLOCK && set!=NULL) {
    for (i=0;i<SIG_MAXNUM;i++) {
      if (sigismember(set,i)) signals[i].masked = 0;
      if (oset!=NULL) {
        if (signals[i].masked) sigaddset(oset,i);
        else sigdelset(oset,i);
      }
    }
    return 0;
  }
  else if (how==SIG_SETMASK && set!=NULL) {
    for (i=0;i<SIG_MAXNUM;i++) {
      signals[i].masked = sigismember(set,i);
      if (oset!=NULL) {
        if (signals[i].masked) sigaddset(oset,i);
        else sigdelset(oset,i);
      }
    }
    return 0;
  }
  else if (oset!=NULL) {
    for (i=0;i<SIG_MAXNUM;i++) {
      if (signals[i].masked) sigaddset(oset,i);
      else sigdelset(oset,i);
    }
    return 0;
  }
  else {
    errno = EINVAL;
    return -1;
  }
}
