#include <procm.h>
#include <paging.h>
#include <memphys.h>
#include <memuser.h>
#include <swap.h>
#include <malloc.h>
#include <syscall.h>

/**
 * Initializes swapping
 *  @return Success?
 */
int swap_init() {
  swap_proc = NULL;
  //if (syscall_create(SYSCALL_SWAP_ENABLE,swap_enable,1)==-1) return -1;
  return 0;
}

/**
 * Enables swapping (Syscall)
 *  @return Success?
 */
int swap_enable(void *buf) {
  if (swap_proc!=NULL) return -1;
  swap_queue = llist_create();
  swap_proc = proc_current;
  swap_buf = buf;
  return 0;
}

/**
 * Calls a swap function
 *  @param op Operation
 *  @param proc Owner of page
 *  @param page Page
 *  @return Success?
 */
int swap_call(int op,proc_t *proc,void *page) {
  swap_call_t *new = malloc(sizeof(swap_call_t));
  if (new!=NULL) {
    new->op = op;
    new->pid = proc->pid;
    new->page = page;
    llist_push(swap_queue,new);
    return 0;
  }
  else return -1;
}

/**
 * Swaps page in
 *  @param proc Onwer of page
 *  @param page Page to swap
 */
int swap_in(proc_t *proc,void *page) {
  if (swap_proc!=NULL) {
    pte_t pte = paging_getpte_pd(page,proc->addrspace->pagedir);
    if (!pte.in_memory && pte.swapped) {
      memuser_load_addrspace(proc->addrspace);
      if (swap_call(SWAP_IN,proc,page)) {
        void *phys = memphys_alloc();
        pte.page = ADDR2PAGE(phys);
        pte.in_memory = 1;
        paging_physwrite(phys,swap_buf,PAGE_SIZE);
        return 0;
      }
    }
  }
  return -1;
}

/**
 * Swaps page out
 *  @param proc Owner of page
 *  @param page Page to swap
 *  @return Success?
 */
int swap_out(proc_t *proc,void *page) {
  if (swap_proc!=NULL) {
    pte_t pte = paging_getpte_pd(page,proc->addrspace->pagedir);
    if (pte.swappable) {
      memuser_load_addrspace(proc->addrspace);
      void *phys = PAGE2ADDR(pte.page);
      if (phys!=NULL && pte.in_memory) {
        paging_physread(swap_buf,phys,PAGE_SIZE);
        if (swap_call(SWAP_OUT,proc,page)!=-1) {
          pte.in_memory = 0;
          pte.swapped = 1;
          memphys_free(page);
          paging_setpte_pd(page,pte,proc->addrspace->pagedir);
          return 0;
        }
      }
    }
  }
  return -1;
}

/**
 * Removes a page from swap
 *  @param proc Owner of page
 *  @param page Page
 *  @return Success?
 */
int swap_remove(proc_t *proc,void *page) {
  if (swap_proc!=NULL) return swap_call(SWAP_REM,proc,page);
  else return -1;
}
