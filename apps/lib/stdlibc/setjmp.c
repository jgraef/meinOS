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

#include <setjmp.h>

int setjmp(jmp_buf env) {
  env[0] = __builtin_return_address(0);
  asm("mov %%esp,%0\n"
      "mov %%ebp,%0\n"
      "mov %%ebx,%0\n"
      "mov %%ecx,%0\n"
      "mov %%edx,%0":"=r"(env[0]),"=r"(env[1]),"=r"(env[2]),"=r"(env[3]),"=r"(env[4]),"=r"(env[5]));
  return 0;
}

void longjmp(jmp_buf env,int val) {
  if (val==0) val = 1;
  asm("mov %0,%%esp\n"::"r"(env[1]));
  asm("mov %0,%%ebp\n"::"r"(env[2]));
  asm("mov %0,%%ebx\n"::"r"(env[3]));
  asm("mov %0,%%ecx\n"::"r"(env[4]));
  asm("mov %0,%%edx\n"::"r"(env[5]));
  asm("mov %0,%%eax\n"::"r"(val));
  asm("jmp *%0"::"r"(env[0]));
}
