#include <sys/types.h>
#include <memkernel.h>
#include <memuser.h>
#include <memphys.h>
#include <memmap.h>
#include <paging.h>
#include <malloc.h>
#include <debug.h>

/**
 * Initializes kernel memory management
 *  @return 0=Success; -1=Failure
 */
void *init_stack;
size_t init_stacksize;
void malloc_init(void *(*getpage)(size_t size),void (*freepage)(void *addr));
void malloc_donatemem(void *addr,size_t size);
int memkernel_init() {
  malloc_init(memkernel_alloc,memkernel_free);
  malloc_donatemem(init_stack,init_stacksize); // donate initial stack to memory allocator
  return 0;
}

/**
 * Finds free virtual pages
 *  @param pages How many pages wanted
 *  @return Address of first page
 */
void *memkernel_findvirt(size_t pages) {
  void *virt;
  size_t found = 0;

  for (virt = (void*)KERNELDATA_ADDRESS;virt<(void*)KERNELDATA_ADDRESS+KERNELDATA_SIZE;virt+=PAGE_SIZE) {
    // check if enough pages are found
    if (found>=pages) return virt-found*PAGE_SIZE;
    if (ADDR2PTE(virt)==0) {
      // check for PDE
      if (!paging_getpde(virt).exists) {
        found += 1024;
        virt += 1024*(PAGE_SIZE-1);
        continue;
      }
    }
    // check for PTE
    if (paging_getpte(virt).exists) found = 0;
    else found++;
  }

  return NULL;
}

/**
 * Allocates memory for kernel allocator
 *  @param size How many memory to allocate (should be devidable by PAGE_SIZE)
 *  @return Address of allocated memory
 */
void *memkernel_alloc(size_t size) {
  size_t pages = ADDR2PAGE(size);
  size_t i;
  void *virt = memkernel_findvirt(pages);

  for (i=0;i<pages;i++) {
    paging_map(virt+i*PAGE_SIZE,memphys_alloc(),0,1);
    if (memuser_inited) memuser_syncpds(virt+i);
  }
  return virt;
}

/**
 * Frees memory (one page) for kernel allocator
 */
void memkernel_free(void *addr) {
  memphys_free(paging_unmap(addr));
}
