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

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <sys/types.h>
#include <stdint.h>
#include <syscalls.h>
#include <cpu.h>

struct syscall {
  int (*func)();
  int numparams;
};

struct syscall syscalls[SYSCALL_MAXNUM];

int syscall_init();
void syscall_handler(uint32_t *stack);
int syscall_create(int cmd,void *func,int numparams);
void syscall_destroy(int cmd);

#endif
