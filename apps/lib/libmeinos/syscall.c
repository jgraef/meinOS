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
#include <stdarg.h>
#include <syscall.h>

int syscall_call(int syscall,size_t numparams,...) {
  va_list args;
  size_t i;
  int ret;

  va_start(args,numparams);
  for (i=0;i<numparams;i++) asm("push %0"::"r"(va_arg(args,int)));
  va_end(args);
  asm("push %0"::"r"(syscall));
  asm("int %1":"=r"(ret):"i"(SYSCALL_INT));
  asm("add %0,%%esp"::"a"((numparams+1)*4));

  return ret;
}
