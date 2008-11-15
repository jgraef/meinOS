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

#include <tss.h>
#include <cpu.h>
#include <gdt.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <debug.h>
#include <procm.h>

/**
 * Initializes TSS
 *  @return 0=Success; -1=Failure
 */
int tss_init() {
  cpu_t *cpu = cpu_this;

  // create TSS
  tss_t *tss = malloc(sizeof(tss_t));
  if (tss==NULL) return -1;
  memset(tss,0,sizeof(tss_t));
  tss->ss0 = IDX2SEL(2,PRIV_KERNEL);
  tss->esp0 = (uint32_t)(cpu->stack+cpu->stacksize-4)+sizeof(struct vm86_segmentregs);
  tss->iopb_offset = offsetof(tss_t,iopb);
  cpu->tss = tss;

  // create TSS descriptor
  int index = GDT_TSSDESC+cpu_getid();
  gdt_set_descriptor(index,sizeof(tss_t)-1,tss,GDT_PRESENT|GDT_TSS,PRIV_USER);
  gdt[index].flags &= 0x0F;

  // load TSS descriptor
  selector_t selector = {
    .index = index,
    .ti = 0,
    .priv = PRIV_KERNEL
  };
  asm("ltr %0"::"a"(selector));

  return 0;
}
