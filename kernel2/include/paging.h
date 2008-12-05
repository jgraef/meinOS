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

#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <limits.h>

/**
 * Gets address by page
 *  @param page Page
 *  @return Address
 */
#define PAGE2ADDR(page) ((void*)(((unsigned int)(page))*PAGE_SIZE))

/**
 * Gets page by address
 *  @param addr Address
 *  @return Page
 */
#define ADDR2PAGE(addr) (((unsigned int)(addr))/PAGE_SIZE)

/**
 * Gets PDE from Address
 *  @param addr Address
 *  @return PDE
 */
#define ADDR2PDE(addr) (ADDR2PAGE(addr)/1024)

/**
 * Gets PTE from Address
 *  @param addr Address
 *  @return PTE
 */
#define ADDR2PTE(addr) (ADDR2PAGE(addr)%1024)

/**
 * Rounds address down to next page border
 *  @param addr Address
 *  @return Down rounded address
 */
#define PAGEDOWN(addr) ((void*)((((unsigned int)(addr))/PAGE_SIZE)*PAGE_SIZE))

/**
 * Rounds address up to next page border
 *  @param addr Address
 *  @return Up rounded address
 */
#define PAGEUP(addr)   ((void*)((((((unsigned int)(addr))-1)/PAGE_SIZE)+1)*PAGE_SIZE))

/**
 * Gets offset in page
 *  @param addr
 *  @return Offset in page
 */
#define PAGEOFF(addr)  (((unsigned int)addr)%PAGE_SIZE)

/**
 * Maps a page (extended)
 *  @param virt Virtual address
 *  @param phys Physical address
 *  @param user Whether page is accessable by user
 *  @param writable Whether page is writable
 *  @return 0=Success; -1=Failure
 *  @todo By default a page should be swappable
 */
#define paging_map(virt,phys,user,writable) paging_map_pd(virt,phys,user,writable,0,0,paging_curpd)

/// Type for Pagedir Entry
typedef struct {
  /// PT exists
  unsigned exists:1;
  /// PT is writable
  unsigned writable:1;
  /// PT is accessable by user
  unsigned user:1;
  /// Write-through (if not set, write-back is enabled)
  unsigned write_though:1;
  /// Cache disabled
  unsigned cache_disabled:1;
  /// Page was accessed
  unsigned accessed:1;
  /// Zero
  unsigned zero:1;
  /// Pagesize
  enum {
    PGSIZE_4K = 0,
    PGSIZE_4M = 1
  } pagesize:1;
  /// Unused
  unsigned :4;
  /// Page where PT is located
  unsigned page:20;
} __attribute__ ((packed)) pde_t;

/// Type for Pagetable Entry
typedef struct {
  /// Page is in memory
  unsigned in_memory:1;
  /// PT is writable
  unsigned writable:1;
  /// PT is accessable by user
  unsigned user:1;
  /// Write-through (if not set, write-back is enabled)
  unsigned write_though:1;
  /// Cache disabled
  unsigned cache_disabled:1;
  /// Page was accessed
  unsigned accessed:1;
  /// Page is dirty
  unsigned dirty:1;
  /// Page is COW
  unsigned cow:1;
  /// Page is global
  unsigned global:1;
  /// Page exists
  unsigned exists:1;
  /// If page is swappable
  unsigned swappable:1;
  /// If page is swapped
  unsigned swapped:1;
  /// Physical page that is mapped
  unsigned page:20;
} __attribute__ ((packed)) pte_t;

/// Type for Pagedir
typedef pde_t* pd_t;

/// Type for Pagetable
typedef pte_t* pt_t;

int paging_enabled;
pd_t paging_curpd;
pd_t paging_kernelpd;
void *paging_physrw_lastpage;
pd_t paging_physrw_lastpd;

int paging_init();
int paging_loadpagedir(pd_t pd);
int paging_physread(void *dest,void *src,size_t count);
int paging_physwrite(void *dest,void *src,size_t count);
void *paging_cleanpage(void *page);
pde_t paging_getpde(void *virt);
int paging_setpde(void *virt,pde_t pde);
pte_t paging_getpte(void *virt);
int paging_setpte(void *virt,pte_t pte);
pde_t paging_getpde_pd(void *virt,pd_t pagedir);
int paging_setpde_pd(void *virt,pde_t pde,pd_t pagedir);
pte_t paging_getpte_pd(void *virt,pd_t pagedir);
int paging_setpte_pd(void *virt,pte_t pte,pd_t pagedir);
int paging_map_pd(void *virt,void *phys,int user,int writable,int swappable,int cow,pd_t pagedir);
void *paging_unmap(void *virt);
void *paging_getphysaddr(void *virt);

static inline void paging_flushtlb(void *addr) {
  asm("invlpg %0"::"m"(addr));
}

#endif
