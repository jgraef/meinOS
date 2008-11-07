#include <sys/types.h>
#include <paging.h>
#include <string.h>
#include <memphys.h>
#include <memmap.h>
#include <debug.h>

/**
 * Initializes Paging
 *  @return 0=Success; -1=Failure
 */
int paging_init() {
  size_t i;

  paging_enabled = 0;
  paging_physrw_lastpage = NULL;
  paging_physrw_lastpd = NULL;

  // write initial pagedir
  paging_kernelpd = (pd_t)KERNELPD_ADDRESS;
  paging_curpd = paging_kernelpd;
  memset(paging_kernelpd,0,PAGE_SIZE);
  paging_kernelpd[0].page = ADDR2PAGE(KERNELPD_ADDRESS+PAGE_SIZE);
  paging_kernelpd[0].pagesize = PGSIZE_4K;
  paging_kernelpd[0].user = 1;
  paging_kernelpd[0].writable = 1;
  paging_kernelpd[0].exists = 1;
  paging_kernelpd[1].page = ADDR2PAGE(KERNELPD_ADDRESS+PAGE_SIZE*2);
  paging_kernelpd[1].pagesize = PGSIZE_4K;
  paging_kernelpd[1].user = 1;
  paging_kernelpd[1].writable = 1;
  paging_kernelpd[1].exists = 1;
  // write initial pagetables
  memset((void*)(KERNELPD_ADDRESS+PAGE_SIZE),0,PAGE_SIZE);
  for (i=0;i<KERNELCODE_ADDRESS+KERNELCODE_SIZE;i+=PAGE_SIZE) {
    if ((i>=IVT_ADDRESS && i<IVT_ADDRESS+IVT_SIZE) || (i>=BIOSCODE_ADDRESS && i<BIOSCODE_ADDRESS+BIOSCODE_SIZE)) paging_map((void*)i,(void*)i,1,0);
    else paging_map((void*)i,(void*)i,0,1);
  }
  // write second pagetables (maybe needed by stack)
  memset((void*)(KERNELPD_ADDRESS+PAGE_SIZE*2),0,PAGE_SIZE);

  // map pagedir as last pagetable
  memset(paging_kernelpd+1023,0,sizeof(pde_t));
  paging_kernelpd[1023].page = ADDR2PAGE(paging_kernelpd);
  paging_kernelpd[1023].pagesize = PGSIZE_4K;
  paging_kernelpd[1023].user = 0;
  paging_kernelpd[1023].writable = 1;
  paging_kernelpd[1023].exists = 1;

  // enable paging
  paging_loadpagedir(paging_kernelpd);
  asm("mov %cr0,%eax; or $0x80000000,%eax; mov %eax,%cr0;");
  paging_enabled = 1;

  return 0;
}

/**
 * Loads pagedir
 *  @param pd Pagedir
 *  @return Success?
 */
int paging_loadpagedir(pd_t pd) {
  asm("mov %0,%%cr3;"::"a"(pd));
  paging_curpd = pd;
  return 0;
}

/**
 * Reads bytes from physical memory
 *  @param dest Destination for read data
 *  @param src Physical address as source
 *  @param count How many bytes to read
 *  @return How many bytes read
 *  @note Read does not work over page borders
 */
int paging_physread(void *dest,void *src,size_t count) {
  void *src_rounded = PAGEDOWN(src);

  if (src_rounded!=paging_physrw_lastpage || paging_curpd!=paging_physrw_lastpd) {
    if (paging_map((void*)BUFPAGE_ADDRESS,src_rounded,0,1)<0) return 0;
    paging_physrw_lastpage = src_rounded;
    paging_physrw_lastpd = paging_curpd;
  }
  memcpy(dest,((void*)BUFPAGE_ADDRESS)+PAGEOFF(src),count);
  return count;
}

/**
 * Writes bytes to physical memory
 *  @param dest Physical address as destination
 *  @param src Source of data to write
 *  @param count How many bytes to write
 *  @return How many bytes written
 *  @note Write does not work over page borders
 *  @todo Enable caching
 */
#include <memuser.h>
int paging_physwrite(void *dest,void *src,size_t count) {
  void *dest_rounded = PAGEDOWN(dest);

  if (dest_rounded!=paging_physrw_lastpage || paging_curpd!=paging_physrw_lastpd || 1) {
    if (paging_map((void*)BUFPAGE_ADDRESS,dest_rounded,0,1)<0) return 0;
    paging_physrw_lastpage = dest_rounded;
    paging_physrw_lastpd = paging_curpd;
  }
  memcpy(((void*)BUFPAGE_ADDRESS)+PAGEOFF(dest),src,count);
  return count;
}

/**
 * Clean a whole page
 *  @param page Physical address of page
 */
void *paging_cleanpage(void *page) {
  if (paging_enabled) {
    void *page_rounded = PAGEDOWN(page);
    if (page_rounded!=paging_physrw_lastpage || paging_curpd!=paging_physrw_lastpd) {
      if (paging_map((void*)BUFPAGE_ADDRESS,page_rounded,0,1)<0) return 0;
      paging_physrw_lastpage = page_rounded;
      paging_physrw_lastpd = paging_curpd;
    }
    memset((void*)BUFPAGE_ADDRESS,0,PAGE_SIZE);
  }
  else memset(page,0,PAGE_SIZE);
  return page;
}

/**
 * Gets a PDE
 *  @param virt Virtual address
 *  @return PDE
 */
pde_t paging_getpde(void *virt) {
  pde_t pde;
  if (paging_enabled) pde = ((pd_t)PAGEDIR_ADDRESS)[ADDR2PDE(virt)];
  else pde = paging_curpd[ADDR2PDE(virt)];
  return pde;
}

/**
 * Sets a PDE
 *  @param virt Virtual address
 *  @param pde PDE
 *  @return 0=Success; -1=Failure
 */
int paging_setpde(void *virt,pde_t pde) {
  if (paging_enabled) ((pd_t)PAGEDIR_ADDRESS)[ADDR2PDE(virt)] = pde;
  else paging_curpd[ADDR2PDE(virt)] = pde;
  paging_flushtlb(virt);
  return 0;
}

/**
 * Gets a PTE
 *  @param virt Virtual address
 *  @return PTE
 */
pte_t paging_getpte(void *virt) {
  pte_t pte;
  pde_t pde = paging_getpde(virt);
  if (!pde.exists) memset(&pte,0,sizeof(pte));
  else {
    if (paging_enabled) pte = ((pt_t)(PAGETABLES_ADDRESS+ADDR2PDE(virt)*PAGE_SIZE))[ADDR2PTE(virt)];
    else pte = ((pt_t)PAGE2ADDR(pde.page))[ADDR2PTE(virt)];
  }
  return pte;
}

/**
 * Sets a PTE
 *  @param virt Virtual address
 *  @param pte PTE
 *  @return 0=Success; -1=Failure
 */
int paging_setpte(void *virt,pte_t pte) {
  pde_t pde = paging_getpde(virt);
  if (!pde.exists) return -1;
  else {
    if (paging_enabled) ((pt_t)(PAGETABLES_ADDRESS+ADDR2PDE(virt)*PAGE_SIZE))[ADDR2PTE(virt)] = pte;
    else ((pt_t)PAGE2ADDR(pde.page))[ADDR2PTE(virt)] = pte;
    paging_flushtlb(virt);
    return 0;
  }
}

/**
 * Gets a PDE from not-loaded pagedir
 *  @param virt Virtual address
 *  @param pagedir Pagedir
 *  @return PDE
 */
pde_t paging_getpde_pd(void *virt,pd_t pagedir) {
  if (pagedir==paging_curpd) return paging_getpde(virt);
  else {
    pde_t pde;
    paging_physread(&pde,pagedir+ADDR2PDE(virt),sizeof(pde));
    return pde;
  }
}

/**
 * Sets a PDE in a not-loaded pagedir
 *  @param virt Virtual address
 *  @param pde PDE
 *  @param pd pagedir
 *  @return 0=Success; -1=Failure
 */
int paging_setpde_pd(void *virt,pde_t pde,pd_t pagedir) {
  if (pagedir==paging_curpd) return paging_setpde(virt,pde);
  else return paging_physwrite(pagedir+ADDR2PDE(virt),&pde,sizeof(pde))==sizeof(pde)?0:-1;
}

/**
 * Gets a PTE from not-loaded pagedir
 *  @param virt Virtual address
 *  @param pagedir Pagedir
 *  @return PTE
 */
pte_t paging_getpte_pd(void *virt,pd_t pagedir) {
  if (pagedir==paging_curpd) return paging_getpte(virt);
  else {
    pt_t pt = PAGE2ADDR(paging_getpde_pd(virt,pagedir).page);
    pte_t pte;
    if (pt==NULL) memset(&pte,0,sizeof(pte));
    else paging_physread(&pte,pt+ADDR2PTE(virt),sizeof(pte));
    return pte;
  }
}

/**
 * Sets a PTE in a not-loaded pagedir
 *  @param virt Virtual address
 *  @param pte PTE
 *  @param pd pagedir
 *  @return 0=Success; -1=Failure
 */
int paging_setpte_pd(void *virt,pte_t pte,pd_t pagedir) {
  if (pagedir==paging_curpd) return paging_setpte(virt,pte);
  else {
    pt_t pt = PAGE2ADDR(paging_getpde_pd(virt,pagedir).page);
    int ret = paging_physwrite(pt+ADDR2PTE(virt),&pte,sizeof(pte))==sizeof(pte)?0:-1;
    paging_flushtlb(virt);
    return ret;
  }
}

/**
 * Maps a page
 *  @param virt Virtual address
 *  @param phys Physical address
 *  @param user Whether page is accessable by user
 *  @param writable Whether page is writable
 *  @return 0=Success; -1=Failure
 */
int paging_map(void *virt,void *phys,int user,int writable) {
  if (!paging_getpde(virt).exists) {
if (memuser_debug==2) kprintf("<A:0x%x><0x%x>\n",paging_getpde(virt),paging_curpd);
    pt_t pagetable = (pt_t)(PAGETABLES_ADDRESS+ADDR2PDE(virt)*PAGE_SIZE);
    pde_t new;
    memset(&new,0,sizeof(new));
    new.page = ADDR2PAGE(memphys_alloc());
    new.pagesize = PGSIZE_4K;
    new.user = 1;
    new.writable = 1;
    new.exists = 1;
    if (paging_setpde(virt,new)<0) return -1;
    paging_flushtlb(pagetable);
if (memuser_debug==1) kprintf("memset(0x%x,0,0x1000)\n<C:0x%x><0x%x>\n",pagetable,new,paging_curpd);
if (memuser_debug==2) while (1);
    memset(pagetable,0,PAGE_SIZE);
  }
  pte_t new;
  memset(&new,0,sizeof(new));
  new.page = ADDR2PAGE(phys);
  new.exists = 1;
  new.user = user?1:0;
  new.writable = writable?1:0;
  new.in_memory = 1;
  new.swappable = 1;
  if (paging_setpte(virt,new)<0) return -1;
  return 0;
}

/**
 * Unmaps a page and returns physical address of it
 *  @param virt Virtual address
 *  @return Physical address
 */
void *paging_unmap(void *virt) {
  void *addr = PAGE2ADDR(paging_getpte(virt).page);
  pte_t pte;
  memset(&pte,0,sizeof(pte));
  paging_setpte(virt,pte);
  return addr;
}

/**
 * Gets physical address
 *  @param virt Virtual address
 *  @return Physical address
 */
void *paging_getphysaddr(void *virt) {
  pte_t pte = paging_getpte(virt);
  if (pte.exists && pte.in_memory) return PAGE2ADDR(pte.page)+PAGEOFF(virt);
  else return NULL;
}
