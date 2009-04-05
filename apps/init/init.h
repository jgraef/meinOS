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

#ifndef _INIT_H_
#define _INIT_H_

#include <sys/types.h>

#include "exe_elf.h"

// Executables

typedef struct {
  void *data;
  enum {
    EXE_ELF
  } type;
} exe_t;

exe_t *exe_create(const char *file);
void *exe_load(exe_t *exe,pid_t pid);
void exe_destroy(exe_t *exe);

// Grub modules

pid_t *init_get_grub_modules();
int init_run_grub_modules(pid_t *modules);
void init_sort_grub_modules(pid_t *modules);

// Proc

int proc_exec(const char *file,int var);
pid_t proc_fork(void *child_entry);

// ProcFS
int procfs_init(pid_t *grub_modules);
int procfs_run(void);
int procfs_proc_new(pid_t pid,const char *name,const char *exe);
int procfs_proc_update(pid_t pid,const char *name,const char *exe);

#endif /* _INIT_H_ */
