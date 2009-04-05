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

#ifndef _MEMUSER_H_
#define _MEMUSER_H_

typedef struct addrspace_S addrspace_t;

#include <paging.h>
#include <llist.h>
#include <procm.h>

struct addrspace_S {
  proc_t *proc;
  llist_t pages_loaded;
  llist_t pages_imaginary; /// @todo this list is unused
  llist_t pages_swapped;
  pd_t pagedir;
  void *stack;
};

int memuser_inited;
int memuser_debug;

int memuser_init();
void *memuser_findvirt(addrspace_t *addrspace,size_t pages);
pd_t memuser_create_pagedir();
int memuser_destroy_pagedir(pd_t pagedir);
pd_t memuser_clone_pagedir(pd_t pagedir);
int memuser_create_pagetable(pd_t pagedir,void *virt);
addrspace_t *memuser_create_addrspace(proc_t *proc);
int memuser_destroy_addrspace(addrspace_t *addrspace);
int memuser_load_addrspace(addrspace_t *addrspace);
void *memuser_alloc(addrspace_t *addrspace,size_t count,int swappable);
void *memuser_alloc_syscall(size_t count);
int memuser_free(addrspace_t *addrspace,void *page);
int memuser_free_syscall(void *page);
void *memuser_getphysaddr(addrspace_t *addrspace,void *virt);
void *memuser_getphysaddr_syscall(void *virt);
void *memuser_create_stack(addrspace_t *addrspace);
int memuser_expand_stack(addrspace_t *addrspace);
int memuser_destroy_stack(addrspace_t *addrspace);
int memuser_pagefault(void *addr);
int memuser_alloc_at(addrspace_t *addrspace,void *addr,void *phys,int writable);
int memuser_syncpds(void *addr);
void *memuser_getvga();
void *memuser_dma_alloc(size_t size);
int memuser_dma_free(void *addr,size_t size);
addrspace_t *memuser_clone_addrspace(proc_t *proc,addrspace_t *addrspace);

#endif
