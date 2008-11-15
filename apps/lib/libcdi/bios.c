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

#include <syscall.h>
#include <stdlib.h>

#include "cdi/bios.h"
#include "cdi/lists.h"

#include <stdio.h>

int cdi_bios_int10(struct cdi_bios_registers *registers,cdi_list_t memory) {
  size_t i;
  unsigned int *meminfo = malloc(cdi_list_size(memory)*3);
  struct cdi_bios_memory *mem;

  for (i=0;(mem = cdi_list_get(memory,i));i++) {
    meminfo[i*3] = mem->dest;
    meminfo[i*3+1] = (unsigned int)mem->src;
    meminfo[i*3+2] = mem->size;
  }

  int ret = syscall_call(SYSCALL_MISC_BIOSINT,10,meminfo,cdi_list_size(memory),registers->ax,registers->bx,registers->cx,registers->dx,registers->si,registers->di,registers->ds,registers->es);
  free(mem);
  return ret;
}
