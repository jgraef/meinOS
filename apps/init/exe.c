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

#include <sys/types.h>
#include <stdlib.h>

#include "init.h"
#include "exe_elf.h"

exe_t *exe_create(const char *file) {
  exe_t *exe = malloc(sizeof(exe_t*));

  // try ELF
  exe->data = elf_create(file);
  if (exe->data!=NULL) {
    exe->type = EXE_ELF;
    return exe;
  }
  else elf_destroy((elf_t*)exe->data);

  // not an executable
  free(exe);
  return NULL;
}

void *exe_load(exe_t *exe,pid_t pid) {
  if (exe->type==EXE_ELF) return elf_load((elf_t*)exe->data,pid);
  else return NULL;
}

void exe_destroy(exe_t *exe) {
  if (exe->type==EXE_ELF) elf_destroy((elf_t*)exe->data);
  free(exe);
}
