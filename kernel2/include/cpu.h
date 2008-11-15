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

#ifndef _CPU_H_
#define _CPU_H_

#include <sys/types.h>
#include <stdint.h>
#include <llist.h>
#include <malloc.h>
#include <tss.h>

#define cpu_getid() 0
#define cpu_this    ((cpu_t*)llist_get(cpus,cpu_getid()))

struct cpu_registers {
  uint32_t *eax;
  uint32_t *ebx;
  uint32_t *ecx;
  uint32_t *edx;
  uint32_t *esi;
  uint32_t *edi;
  uint32_t *ebp;
  uint32_t *esp;
  uint32_t *eip;
  uint32_t *efl;
  uint32_t *cs;
  uint32_t *ds;
  uint32_t *es;
  uint32_t *fs;
  uint32_t *gs;
  uint32_t *ss;
};

typedef struct {
  unsigned int id;
  int enabled;
  void *stack;
  size_t stacksize;
  struct cpu_registers regs;
  tss_t *tss;
  int uselapic;
  clock_t interval; // all 10 ms
  clock_t ticks;
} cpu_t;

llist_t cpus;

static inline void cpu_halt() {
  asm("hlt");
}

int cpu_init();
int cpu_stack_create(cpu_t *cpu,size_t size);
void cpu_id(uint32_t seax,uint32_t *deax,uint32_t *debx,uint32_t *decx,uint32_t *dedx);
void cpu_shutdown();

#endif
