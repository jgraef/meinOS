#include <sys/types.h>
#include <elf.h>
#include <paging.h>
#include <memuser.h>
#include <memmap.h>
#include <string.h>
#include <debug.h>
#include <cpu.h>

int elf_validate(elf_header_t *header) {
  if (header->ident[EI_MAG0]!=ELFMAG0 || header->ident[EI_MAG1]!=ELFMAG1 || header->ident[EI_MAG2]!=ELFMAG2 || header->ident[EI_MAG3]!=ELFMAG3) return -1;
  if (header->machine!=EM_386) return -1;
  if (header->ident[EI_CLASS]!=ELFCLASS32) return -1;
  if (header->ident[EI_DATA]!=ELFDATA2LSB) return -1;
  if (header->ident[EI_VERSION]!=header->version) return -1;
  return 0;
}

/**
 * Loads an ELF segment in address space
 *  @param memaddr Destination address in memory
 *  @param memsize Size in memory
 *  @param fileaddr Source address in file (absolute)
 *  @param filesize Size in file
 *  @param addrspace User address space
 *  @param writable If segment is writeable
 */
int elf_loadseg(void *memaddr,size_t memsize,void *fileaddr,size_t filesize,addrspace_t *addrspace,int writable) {
  size_t i;

  if (memaddr<(void*)USERDATA_ADDRESS) return -1;
  if (memuser_load_addrspace(addrspace)==-1) return -1;
  for (i=0;i<memsize;i+=PAGE_SIZE) {
    if (memuser_alloc_at(addrspace,memaddr+i,i<filesize?paging_getphysaddr(fileaddr+i):NULL,writable)==-1) return -1;
  }
  if (filesize<memsize) memset(memaddr+filesize,0,memsize-filesize);
  return 0;
}

/**
 * Loads an ELF file in an address space
 *  @param addrspace Address space
 *  @param file Pointer to ELF file
 *  @param filesize Size of ELF file
 *  @return Entrypoint
 */
void *elf_load(addrspace_t *addrspace,void *file,size_t filesize) {
  elf_header_t *header = file;
  elf_progheader_t *progheader;
  void *entrypoint;
  size_t i;

  if (elf_validate(header)==-1) return NULL;

  entrypoint = (void*)(header->entry);
  progheader = file+header->phoff;

  for (i=0;i<header->phnum;i++) {
    if (progheader[i].type==PT_LOAD) {
      if (elf_loadseg((void*)(progheader[i].vaddr),progheader[i].memsz,file+progheader[i].offset,progheader[i].filesz,addrspace,(progheader[i].flags&PF_W)==PF_W?1:0)==-1) return NULL;
    }
  }

  return entrypoint;
}
