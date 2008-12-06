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

#include <debug.h>
#include <stdint.h>
#include <multiboot.h>
#include <kprint.h>
#include <vga.h>
#include <syscall.h>
#include <paging.h>
#include <memmap.h>
#include <memphys.h>
#include <memkernel.h>
#include <gdt.h>
#include <idt.h>
#include <cpu.h>
#include <interrupt.h>
#include <lapic.h>
#include <tss.h>
#include <ipc.h>
#include <memuser.h>
#include <procm.h>
#include <syscall.h>
#include <elf.h>
#include <string.h>
#include <llist.h>
#include <rpc.h>
#include <signal.h>
#include <ioport.h>
#include <biosint.h>
#include <perm.h>

char *basename(char *path) {
  size_t i;
  char *basename = path;
  for (i=0;path[i];i++) {
    if (path[i]=='/') basename = path+i;
  }
  if (*basename==0) return basename;
  else return basename+1;
}

int fourtytwo() {
  return 42;
}

int putsn(int out,char *buf,size_t maxlen) {
  size_t i;
  for (i=0;buf[i]!=0 && i<maxlen;i++) {
    if (out==0) kprintchar(buf[i]);
    else com_send(buf[i]);
  }
  return i;
}

/**
 * Initializes and runs kernel
 *  @param mbi Multiboot Info
 *  @param magic Multiboot magic number
 *  @return Should not return
 */
int main(multiboot_info_t *mbi,uint32_t magic) {
  vga_init();

  kprintf("meinOS\n\n");

  if (magic!=MULTIBOOT_MAGIC) panic("Not booted with a multiboot bootloader.\n");

  memuser_inited = 0;
  test(multiboot_init(mbi));
  test(paging_init());
  test(memphys_init());
  test(memkernel_init());
  test(syscall_init());
  test(cpu_init());
  test(gdt_init());
  test(idt_init());
  test(interrupt_init());
  test(lapic_init());
  test(tss_init());
  test(ioport_init());
  test(ipc_init());
  test(rpc_init());
  test(signal_init());
  test(memuser_init());
  test(proc_init());
  test(biosint_init());

  /// @deprecated Only for debugging
  syscall_create(SYSCALL_PUTSN,putsn,3);
  syscall_create(SYSCALL_FOURTYTWO,fourtytwo,0);

  // load initial programs
  kprintf("Loading initial programs...\n");
  size_t i,size;
  void *addr;
  char *file;
  char *name;
  proc_t *proc_init;

  for (i=0;(addr = multiboot_get_mod(i,&file,&size));i++) {
    name = basename(file);
    kprintf("    %s:\t%s:\t0x%x / 0x%x...",name,file,addr,size);
    proc_t *new = proc_create(name,PERM_ROOTUID,PERM_ROOTGID,i==0?NULL:proc_init,(i==0),(i==0));
    if (i==0) proc_init = new;
    if (new!=NULL) {
      void *entrypoint = elf_load(new->addrspace,addr,size);
      if (entrypoint!=NULL) {
        new->registers.eip = (uint32_t)entrypoint;
        new->registers.esp = (uint32_t)memuser_create_stack(new->addrspace);
        kprintf("(pid=%d) done\n",new->pid);
        continue;
      }
      else proc_destroy(new);
    }
    kprintf("failed\n");
  }

  proc_idle();
  return 0;
}
