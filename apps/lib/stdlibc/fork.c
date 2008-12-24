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
#include <rpc.h>

#include <stdio.h>

int *_fork_stack;
pid_t _fork_child_entry();

pid_t fork() {
  asm("mov %%ebp,%0":"=r"(_fork_stack):);
  //printf("test: stack: 0x%x\n",_fork_stack);
  return rpc_call("proc_fork",0,_fork_child_entry,-1);
}