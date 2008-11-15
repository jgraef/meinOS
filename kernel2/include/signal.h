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
