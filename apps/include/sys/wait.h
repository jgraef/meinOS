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

#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <sys/types.h>
#include <syscall.h>

#define WCONTINUED 1
#define WNOHANG    2
#define WUNTRACED  4

static __inline__ pid_t waitpid(pid_t pid,int *stat_loc,int options) {
  return syscall_call(SYSCALL_PROC_WAITPID,3,pid,stat_loc,options);
}

static __inline__ pid_t wait(int *stat_loc) {
  return waitpid(-1,stat_loc,0);
}

#endif
