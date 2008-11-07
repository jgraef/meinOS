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
