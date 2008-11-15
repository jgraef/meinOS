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
#include <stdint.h>
#include <cpu.h>
#include <memkernel.h>
#include <paging.h>
#include <llist.h>
#include <sizes.h>
#include <idt.h>
#include <debug.h>
#include <interrupt.h>
#include <vga.h>

/**
 * Initializes CPUs
 *  @return 0=Success; -1=Failure
 */
int cpu_init() {
  if ((cpus = llist_create())==NULL) return -1;
  cpu_t *cpu = malloc(sizeof(cpu_t));
  if (cpu==NULL) return -1;
  cpu->enabled = 1;
  cpu_stack_create(cpu,PAGE_SIZE);
  llist_push(cpus,cpu);
  return 0;
}
/**
 * Creates stack
 *  @param cpu CPU to create stack for
 *  @param size Stack size
 *  @return 0=Success; -1=Failure
 */
int cpu_stack_create(cpu_t *cpu,size_t size) {
  if ((cpu->stack = memkernel_alloc((size_t)PAGEUP(size)))==NULL) return -1;
  cpu->stacksize = size;
  return 0;
}

void cpu_id(uint32_t seax,uint32_t *deax,uint32_t *debx,uint32_t *decx,uint32_t *dedx) {
  uint32_t eax,ebx,ecx,edx;
  asm("cpuid":"=a"(eax),"=b"(ebx),"=c"(ecx),"=d"(edx):"a"(seax));
  if (deax!=NULL) *deax = eax;
  if (debx!=NULL) *debx = ebx;
  if (decx!=NULL) *decx = ecx;
  if (dedx!=NULL) *dedx = edx;
}

/**
 * Shuts down computer
 */
void cpu_shutdown() {
  //vga_text_clear();
  kprintf("Please turn off the computer\n");
  interrupt_enable(0);
  cpu_halt();
}

/**
 * Resets computer
 */
void cpu_restart() {
  idtsel_t idtsel = {
    .size = 0,
    .offset = 0
  };
  asm("lidt (%0)"::"r"(&idtsel));
}
