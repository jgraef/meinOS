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

#ifndef _SWAP_H_
#define _SWAP_H_

#include <sys/types.h>
#include <procm.h>

#define SWAP_IN  1
#define SWAP_OUT 2
#define SWAP_REM 3

typedef struct {
  int op;
  pid_t pid;
  void *page;
} swap_call_t;

llist_t swap_queue;
proc_t *swap_proc;
void *swap_buf;

int swap_init();
int swap_enable(void *buf);
int swap_call(int op,proc_t *proc,void *page);
int swap_in(proc_t *proc,void *page);
int swap_out(proc_t *proc,void *page);
int swap_remove(proc_t *proc,void *page);

#endif
