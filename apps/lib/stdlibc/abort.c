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
#include <stdio.h>
#include <proc.h>
#include <unistd.h>

void _close_all_filehandles(); ///< @see files.c

/**
 * Aborts the program
 *  @todo Save cordump to file "core"
 */
void abort() {
  #ifdef COREFILE
    FILE *fd = file(COREFILE,"w");
    if (fd==NULL) fd = stderr;
  #else
    FILE *fd = stderr;
  #endif

  pid_t pid = getpid();
  fprintf(fd,"Process \"%s\" (PID: %d) was aborted\n",getname(pid),pid);
  fprintf(fd,"Aborted at 0x%x\n",__builtin_return_address(1));
  fprintf(fd,"Strack frame: 0x%x\n",__builtin_frame_address(0));

  _close_all_filehandles();
  syscall_call(SYSCALL_PROC_ABORT,0);
}
