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
#include <memmap.h>
#include <memphys.h>
#include <multiboot.h>
#include <paging.h>
#include <limits.h>
#include <debug.h>
#include <string.h>
#include <memkernel.h>
#include <kprint.h>

/**
 * Initializes physical memory management
 *  @return -1=Success; 0=Failure
 */
int memphys_init() {
  memphys_memory = (multiboot_get_memlower()+multiboot_get_memlower());

  memphys_stackbase = (void*)STACKDATA_ADDRESS;
  memphys_stackend = memphys_stackbase;
  memphys_stackptr = memphys_stackbase;

  // push free memory on stack
  size_t i = 0;
  void *addr;
  size_t size;
  multiboot_mmap_type_t type;
  while (multiboot_get_mmap(i++,&addr,&size,&type)==0) {
    if (type==MULTIBOOT_FREE) {
      size_t j;
      for (j=0;j<size/PAGE_SIZE;j++) {
        if (addr+j*PAGE_SIZE>=(void*)FREEPHYS_ADDRESS) memphys_push(addr+j*PAGE_SIZE);
      }
    }
  }

  return 0;
}

/**
 * Pushs an address on stack
 *  @param addr Address to push on stack
 */
void memphys_push(void *addr) {
  if (memphys_stackptr==memphys_stackend) {
    // use pushed page as stackpage
    if (paging_map(memphys_stackend,addr,0,1)<0) panic("Error while mapping stackpage: 0x%x",memphys_stackend);
    memphys_stackend += PAGE_SIZE/sizeof(void*);
  }
  else {
    // just push page on stack
    *memphys_stackptr = addr;
    memphys_stackptr++;
  }
}

/**
 * Pops an address from stack
 *  @return Address popped from stack
 *  @todo Check whether address is already used for DMA
 */
void *memphys_pop() {
  void *page = NULL;
  if (memphys_stackptr>memphys_stackbase) {
    if (memphys_stackptr-memphys_stackend>PAGE_SIZE) {
      // free stackpage and return it
      memphys_stackend -= PAGE_SIZE;
      paging_unmap(memphys_stackend);
      page = memphys_stackend;
    }
    else {
      // just get page from stack
      memphys_stackptr--;
      page = *memphys_stackptr;
    }
  }
  else panic("Ran out of memory\n");
  return page;
}

/**
 * Returns the amount of remaining memory
 *  @return Amount of remaining memory in bytes
 */
size_t memphys_memleft() {
  return (((size_t)memphys_stackptr)-((size_t)memphys_stackbase))*PAGE_SIZE/4;
}

/**
 * Initializes DMA management
 *  @return Success?
 */
int memphys_dma_init() {
  memphys_dma_bitmap_size = MEMPHYS_DMALIMIT/(PAGE_SIZE*8);
  memphys_dma_bitmap = malloc(memphys_dma_bitmap_size);
  // memory until free memory (BDA, Kernel Code, Stack)
  memset(memphys_dma_bitmap,0xFF,FREEPHYS_ADDRESS/(PAGE_SIZE*8));
  // free pages
  memset(((void*)memphys_dma_bitmap)+FREEPHYS_ADDRESS/(PAGE_SIZE*8),0,memphys_dma_bitmap_size-FREEPHYS_ADDRESS/(PAGE_SIZE*8));
  return 0;
}

/**
 * Gets free DMA pages
 *  @param pages Amount of requested pages
 *  @return Address of first page
 */
void *memphys_dma_alloc(size_t pages) {
  size_t i;
  size_t found = 0;
  for (i=0;i<MEMPHYS_DMALIMIT/PAGE_SIZE;i++) {
    if (!(memphys_dma_bitmap[i/(8*sizeof(int))]&(1<<(i%(8*sizeof(int)))))) found++;
    else found = 0;
    if (found==pages) {
      size_t j;
      for (j=i-(found-1);j<=i;j++) memphys_dma_bitmap[j/(8*sizeof(int))] |= 1<<(j%(8*sizeof(int)));
      return (void*)((i-(found-1))*PAGE_SIZE);
    }
  }
  return NULL;
}

/**
 * Frees a DMA page
 *  @param addr Address of DMA page
 */
void memphys_dma_free(void *addr) {
  size_t i = ((size_t)addr)/PAGE_SIZE;
  memphys_dma_bitmap[i/(8*sizeof(int))] &= ~(1<<(i%(8*sizeof(int))));
}
