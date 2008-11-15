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
#include <memuser.h>
#include <llist.h>
#include <malloc.h>
#include <swap.h>
#include <memphys.h>
#include <paging.h>
#include <memmap.h>
#include <procm.h>
#include <string.h>
#include <sizes.h>
#include <syscall.h>
#include <debug.h>
#include <perm.h>
#include <vga.h>

/**
 * Initializes User Memory Management
 *  @return Success?
 */
int memuser_init() {
  memuser_inited = 1;
memuser_debug = 0;
  if (syscall_create(SYSCALL_MEM_MALLOC,memuser_alloc_syscall,1)==-1) return -1;
  if (syscall_create(SYSCALL_MEM_FREE,memuser_free_syscall,1)==-1) return -1;
  if (syscall_create(SYSCALL_MEM_GETPHYSADDR,memuser_getphysaddr_syscall,1)==-1) return -1;
  if (syscall_create(SYSCALL_MEM_GETVGA,memuser_getvga,1)==-1) return -1;
  if (syscall_create(SYSCALL_MEM_DMA_ALLOC,memuser_dma_alloc,1)==-1) return -1;
  if (syscall_create(SYSCALL_MEM_DMA_FREE,memuser_dma_free,2)==-1) return -1;
  if (memphys_dma_init()==-1) return -1;
  return swap_init();
}

/**
 * Find a free virtual address
 *  @param addrspace Address space
 *  @param pages Number of pages
 *  @return Address of first page
 */
void *memuser_findvirt(addrspace_t *addrspace,size_t pages) {
  void *virt;
  size_t found = 0;

  for (virt = (void*)USERDATA_ADDRESS;virt<(void*)USERDATA_ADDRESS+USERDATA_SIZE;virt+=PAGE_SIZE) {
    // check if enough pages are found
    if (found>=pages) return virt-found*PAGE_SIZE;
    if (ADDR2PTE(virt)==0) {
      // check for PDE
      if (!paging_getpde_pd(virt,addrspace->pagedir).exists) {
        found += 1024;
        virt += 1023*PAGE_SIZE;
        continue;
      }
    }
    // check for PTE
    if (paging_getpte_pd(virt,addrspace->pagedir).exists) found = 0;
    else found++;
  }

  return NULL;
}

/**
 * Creates an user pagedir
 *  @return User pagedir
 */
pd_t memuser_create_pagedir() {
if (memuser_debug) kprintf("HELLO\n");
  pd_t pagedir = paging_cleanpage(memphys_alloc());
  // copy kernel PTEs
  pde_t pde;
  paging_physwrite(pagedir,(void*)PAGEDIR_ADDRESS,ADDR2PDE(KERNELDATA_ADDRESS+KERNELDATA_SIZE));
  // link last PDE to PD
  memset(&pde,0,sizeof(pde));
  pde.page = ADDR2PAGE(pagedir);
  pde.pagesize = PGSIZE_4K;
  pde.user = 0;
  pde.writable = 1;
  pde.exists = 1;
  paging_setpde_pd((void*)(4092*MBYTES),pde,pagedir);
  return pagedir;
}

/**
 * Destroys an user pagedir
 *  @param pagedir Pagedir
 *  @return Success?
 *  @todo Check if there are really no user pages left
 */
int memuser_destroy_pagedir(pd_t pagedir) {
  if (paging_curpd==pagedir) paging_loadpagedir(paging_kernelpd);
  memphys_free(pagedir);
  return 0;
}

/**
 * Copies Pagedir
 *  @param pagedir Pagedir to be copied (must be loaded)
 *  @return New pagedir
 */
pd_t memuser_clone_pagedir(pd_t pagedir) {
  if (paging_curpd!=pagedir) return NULL;


  /*kprintf("FIXME: %s line %d\n",__FILE__,__LINE__);
  kprintf("Fork crashes memphys, so memphys thinks it has no memory left\n");
while (1);*/

  pd_t new = memuser_create_pagedir();

  // copy user PDEs,PTs,PTEs and pages
  size_t i;
  for (i=USERDATA_ADDRESS;i<USERDATA_SIZE;i+=PAGE_SIZE) {
    if (i%1024*PAGESIZE==0) memuser_create_pagetable(new,(void*)i);
    pte_t pte = paging_getpte((void*)i);
    if (pte.in_memory) {
      kprintf("Left:     0x%x\n",memphys_memleft());
      pte.page = ADDR2PAGE(memphys_alloc());
      kprintf("New page: 0x%x\n",PAGE2ADDR(pte.page));
      paging_physwrite(PAGE2ADDR(pte.page),(void*)i,PAGE_SIZE);
    }
    paging_setpte_pd((void*)i,pte,new);
  }
  return new;
}

/**
 * Creates an user pagetable
 *  @param addrspace Address space
 *  @param virt Virtual address
 *  @return Success?
 */
int memuser_create_pagetable(pd_t pagedir,void *virt) {
  pde_t new;
  memset(&new,0,sizeof(new));
memuser_debug++;
  new.page = ADDR2PAGE(paging_cleanpage(memphys_alloc()));
memuser_debug--;
  new.pagesize = PGSIZE_4K;
  new.user = 1;
  new.writable = 1;
  new.exists = 1;
  return paging_setpde_pd(virt,new,pagedir)>=0?0:-1;
}

/**
 * Creates an address space
 *  @return Address space
 */
addrspace_t *memuser_create_addrspace(proc_t *proc) {
  addrspace_t *new = malloc(sizeof(addrspace_t));
  new->proc = proc;
  new->pages_loaded = llist_create();
  new->pages_imaginary = llist_create();
  new->pages_swapped = llist_create();
  new->pagedir = memuser_create_pagedir();
  new->stack = NULL;
  return new;
}

/**
 * Destroys an address space
 *  @param addrspace Address space
 *  @return Success?
 */
int memuser_destroy_addrspace(addrspace_t *addrspace) {
  if (paging_curpd!=addrspace->pagedir) return -1;
  void *page;

  if (addrspace->stack!=NULL) memuser_destroy_stack(addrspace->stack);
  while ((page = llist_pop(addrspace->pages_loaded))!=NULL) memphys_free(paging_unmap(page));
  while ((page = llist_pop(addrspace->pages_imaginary))!=NULL) paging_unmap(page);
  while ((page = llist_pop(addrspace->pages_swapped))!=NULL) swap_remove(addrspace->proc,page);
  llist_destroy(addrspace->pages_loaded);
  llist_destroy(addrspace->pages_imaginary);
  llist_destroy(addrspace->pages_swapped);
  memuser_destroy_pagedir(addrspace->pagedir);
  return 0;
}

/**
 * Loads an address space
 *  @param addrspace Address space
 *  @return Success?
 */
int memuser_load_addrspace(addrspace_t *addrspace) {
  if (paging_curpd!=addrspace->pagedir) return paging_loadpagedir(addrspace->pagedir);
  else return 0;
}

/**
 * Allocates an user page
 *  @param addrspace Address space
 *  @param count How many bytes to allocated (should be devidable with PAGE_SIZE)
 *  @return Address
 */
void *memuser_alloc(addrspace_t *addrspace,size_t count,int swappable) {
  void *addr = memuser_findvirt(addrspace,count/PAGE_SIZE);
  if (addr!=NULL) {
    size_t i;
    for (i=0;i<count/PAGE_SIZE;i++) {
      void *virt = addr+i*PAGE_SIZE;
      if (!paging_getpde_pd(virt,addrspace->pagedir).exists) memuser_create_pagetable(addrspace->pagedir,virt);
      pte_t pte;
      memset(&pte,0,sizeof(pte));
      pte.exists = 1;
      pte.swappable = swappable;
      pte.swapped = 0;
      pte.writable = 1;
      pte.user = 1;
      paging_setpte_pd(virt,pte,addrspace->pagedir);
      llist_push(addrspace->pages_imaginary,virt);
    }
  }
  return addr;
}

/**
 * Allocates an user page (Syscall)
 *  @param count How many bytes to allocated (should be devidable with PAGE_SIZE)
 *  @return Address
 */
void *memuser_alloc_syscall(size_t count) {
  return memuser_alloc(proc_current->addrspace,count,1);
}

/**
 * Frees an user page
 *  @param addrspace Address space
 *  @param page Page
 *  @return Success?
 */
int memuser_free(addrspace_t *addrspace,void *page) {
  if (page==NULL) return 0;

  pte_t pte = paging_getpte_pd(addrspace->stack,addrspace->pagedir);
  if (pte.in_memory) {
    memphys_free(PAGE2ADDR(pte.page));
    llist_remove(addrspace->pages_loaded,llist_find(addrspace->pages_loaded,page));
  }
  else if (pte.swapped) {
    swap_remove(proc_current,page);
    llist_remove(addrspace->pages_swapped,llist_find(addrspace->pages_swapped,page));
  }
  else {
    paging_unmap(page);
    llist_remove(addrspace->pages_imaginary,llist_find(addrspace->pages_imaginary,page));
  }
  memset(&pte,0,sizeof(pte));
  paging_setpte_pd(page,pte,addrspace->pagedir);
  return 0;
}

/**
 * Frees an user page (Syscall)
 *  @param page Page
 *  @return Success?
 */
int memuser_free_syscall(void *page) {
  return memuser_free(proc_current->addrspace,page);
}

/**
 * Gets physical address of a virtual one in specified address space
 *  @param addrspace Address space
 *  @param virt Virtual address
 *  @return Physical address
 */
void *memuser_getphysaddr(addrspace_t *addrspace,void *virt) {
  pte_t pte = paging_getpte_pd(virt,addrspace->pagedir);
  if (!pte.in_memory) return NULL;
  return PAGE2ADDR(pte.page);
}

/**
 * Gets physical address of a virtual one in specified address space (Syscall)
 *  @param virt Virtual address
 *  @return Physical address
 */
void *memuser_getphysaddr_syscall(void *virt) {
  return memuser_getphysaddr(proc_current->addrspace,virt);
}

/**
 * Creates a stack
 *  @param addrspace Address space
 *  @return Stack address
 */
void *memuser_create_stack(addrspace_t *addrspace) {
  addrspace->stack = memuser_alloc(addrspace,PAGE_SIZE,0);
  if (addrspace->stack!=NULL) return addrspace->stack+PAGE_SIZE-4;
  else return NULL;
}

/**
 * Destroys a stack
 *  @param addrspace Address space
 *  @return Success?
 */
int memuser_destroy_stack(addrspace_t *addrspace) {
  return memuser_free(addrspace,addrspace->stack);
}

/**
 * User memory pagefault handler
 *  @param addr Address
 *  @return If Pagefault is a "real" Pagefault
 */
int memuser_pagefault(void *addr) {
  addrspace_t *addrspace = proc_current->addrspace;
  void *page = PAGEDOWN(addr);
  pte_t pte = paging_getpte(page);
  if (!pte.exists || pte.in_memory) return -1;
  else if (pte.swapped) {
    if (swap_in(proc_current,page)!=-1) {
      llist_remove(addrspace->pages_swapped,llist_find(addrspace->pages_swapped,page));
      llist_push(addrspace->pages_loaded,page);
      swap_remove(proc_current,page);
      return 0;
    }
  }
  else {
    pte.page = ADDR2PAGE(memphys_alloc());
    pte.in_memory = 1;
    paging_setpte(page,pte);
    llist_remove(addrspace->pages_imaginary,llist_find(addrspace->pages_imaginary,page));
    llist_push(addrspace->pages_loaded,page);
    return 0;
  }
  return -1;
}

/**
 * Allocates a specified page in address space
 *  @param addrspace Address space
 *  @param addr Address to allocate
 *  @param writable If page is writable
 *  @return Success?
 */
int memuser_alloc_at(addrspace_t *addrspace,void *addr,void *phys,int writable) {
  if (!paging_getpde_pd(addr,addrspace->pagedir).exists) {
    memuser_create_pagetable(addrspace->pagedir,addr);
  }

  pte_t pte = paging_getpte_pd(addr,addrspace->pagedir);
/*  if (pte.exists) {
kprintf("foo: 0x%x 0x%x 0x%x\n",addr,PAGE2ADDR(pte.page),phys);
    return -1;
  }*/
  memset(&pte,0,sizeof(pte));
  pte.exists = 1;
  pte.in_memory = 1;
  pte.swappable = 1;
  pte.writable = writable;
  pte.user = 1;
  if (phys==NULL) phys = memphys_alloc();
  pte.page = ADDR2PAGE(phys);
  paging_setpte_pd(addr,pte,addrspace->pagedir);
  return 0;
}

/**
 * Syncronize current pagedir with all other pagedirs
 */
int memuser_syncpds(void *addr) {
  size_t i;
  proc_t *proc;
  pde_t pde = paging_getpde(addr);

  for (i=0;(proc = llist_get(proc_all,i));i++) {
    memuser_load_addrspace(proc->addrspace);
    paging_setpde(addr,pde);
  }
  if (proc_current!=NULL) memuser_load_addrspace(proc_current->addrspace);

  return 0;
}

/**
 * Maps VGA memory in address space
 *  @return Success?
 */
void *memuser_getvga() {
  void *virt;
  if ((virt = memuser_findvirt(proc_current->addrspace,1))) paging_map(virt,(void*)VGA_TEXT_ADDRESS,1,proc_current->gid==PERM_ROOTGID);
  return virt;
}

/**
 * Allocates DMA memory and maps it into address space (Syscall)
 *  @param size Size of DMA memory
 *  @return DMA memory
 */
void *memuser_dma_alloc(size_t size) {
  void *phys = memphys_dma_alloc(size/PAGE_SIZE);
  if (phys!=NULL) {
    void *virt = memuser_findvirt(proc_current->addrspace,size/PAGE_SIZE);
    if (virt!=NULL) {
      size_t i;
      for (i=0;i<size;i+=PAGE_SIZE) {
        paging_map(virt+i,phys+i,1,1);
        //llist_push(proc_current->addrspace->pages_loaded,virt+i);
      }
      return virt;
    }
  }
  return NULL;
}

/**
 * Unmaps DMA memory and frees it (Syscall)
 *  @param addr DMA memory
 *  @param size Size of DMA memory
 */
int memuser_dma_free(void *addr,size_t size) {
  size_t i;
  for (i=0;i<size;i+=PAGE_SIZE) {
    //llist_remove(proc_current->addrspace->pages_loaded,llist_find(proc_current->addrspace->pages_loaded,addr));
    memphys_dma_free(paging_unmap(addr+i));
  }
  return 0;
}

/**
 * Clones an address space
 *  @param addrspace Address space to be clone
 *  @return New address space
 */
addrspace_t *memuser_clone_addrspace(proc_t *proc,addrspace_t *addrspace) {
  addrspace_t *new = malloc(sizeof(addrspace_t));
  new->proc = proc;
  new->pages_loaded = llist_copy(addrspace->pages_loaded);
  new->pages_imaginary = llist_copy(addrspace->pages_imaginary);
  new->pages_swapped = llist_copy(addrspace->pages_swapped);
  new->pagedir = memuser_clone_pagedir(addrspace->pagedir);
  return new;
}
