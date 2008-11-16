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

#include <syscall.h>
#include <biosint.h>
#include <stdint.h>
#include <debug.h>
#include <memmap.h>
#include <procm.h>

#define IVT_GET_IP(i) (((uint16_t*)IVT_ADDRESS)[i*2])
#define IVT_GET_CS(i) (((uint16_t*)IVT_ADDRESS)[i*2+1])

int biosint_init() {
  if (syscall_create(SYSCALL_MISC_BIOSINT,biosint_call,10)==-1) return -1;
  return 0;
}

uint16_t biosint_call(unsigned int *meminfo,size_t meminfo_count,uint16_t ax,uint16_t bx,uint16_t cx,uint16_t dx,uint16_t si,uint16_t di,uint16_t ds,uint16_t es) {
  size_t i;
  proc_t *proc = vm86_proc_create("bios_int",proc_current->uid,proc_current->gid,proc_current);

  proc->registers.eax = ax;
  proc->registers.ebx = bx;
  proc->registers.ecx = cx;
  proc->registers.edx = dx;
  proc->registers.esi = si;
  proc->registers.edi = di;
  proc->registers.ds = ds;
  proc->registers.es = es;

  proc->registers.eip = IVT_GET_IP(0x10);
  proc->registers.cs = IVT_GET_CS(0x10);

  kprintf("Process: %s\n",proc_current->name);
  kprintf("AX: 0x%x\n",proc->registers.eax);
  kprintf("BX: 0x%x\n",proc->registers.ebx);
  kprintf("CX: 0x%x\n",proc->registers.ecx);
  kprintf("DX: 0x%x\n",proc->registers.edx);
  kprintf("SI: 0x%x\n",proc->registers.esi);
  kprintf("DI: 0x%x\n",proc->registers.edi);
  kprintf("DS: 0x%x\n",proc->registers.ds);
  kprintf("ES: 0x%x\n",proc->registers.es);
  kprintf("IP: 0x%x\n",proc->registers.eip);
  kprintf("CS: 0x%x\n",proc->registers.cs);

  for (i=0;i<meminfo_count*3;i+=3) {
    kprintf("Dest: 0x%x\n",meminfo[i]);
    kprintf("Src:  0x%x\n",meminfo[i+1]);
    kprintf("Size: 0x%x\n",meminfo[i+2]);
  }

  proc_sleep(proc_current);

  return -1;
}