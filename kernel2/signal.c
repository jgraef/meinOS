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
