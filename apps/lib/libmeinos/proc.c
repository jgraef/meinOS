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

/**
 * Gets name of a process
 *  @param pid PID of process
 *  @return Name of process (can be passed to free())
 */
char *getname(pid_t pid) {
  size_t size = syscall_call(SYSCALL_PROC_GETNAME,3,pid,NULL,0);
  char *buf = malloc(size);
  syscall_call(SYSCALL_PROC_GETNAME,3,pid,buf,size);
  return buf;
}

void **proc_mempagelist(pid_t pid,size_t *_num_pages) {
  ssize_t num_pages = syscall_call(SYSCALL_PROC_MEMPAGELIST,3,pid,NULL,0);
  if (num_pages>0) {
    void **pages = malloc(num_pages*sizeof(void*));
    *_num_pages = syscall_call(SYSCALL_PROC_MEMPAGELIST,3,pid,pages,num_pages);
    return pages;
  }
  else return NULL;
}
