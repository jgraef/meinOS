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
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <misc.h>
#include <proc.h>
#include <limits.h>

#include "exe_elf.h"

#if 0 /** @todo remove? */
static int elf_loadseg(pid_t pid,int fh,void *mem_addr,size_t mem_size,size_t file_addr,size_t file_size,int writable) {
  if (mem_addr<(void*)USERSPACE_ADDRESS) return -1;
  size_t i;
  int ret = 0;
  pid_t own_pid = getpid();

  lseek(fh,file_addr,SEEK_SET);

  for (i=0;i<mem_size;i+=PAGE_SIZE) {
    void *buf = mem_alloc(PAGE_SIZE);
    size_t cur_count = i>file_size?0:(i+PAGE_SIZE>file_size?file_size%PAGE_SIZE:PAGE_SIZE);
    if (cur_count>0) {
      read(fh,buf,cur_count);
      dbgmsg("Loading from disk: 0x%x 0x%x 0x%x\n",mem_addr+i,file_addr+i,cur_count);
    }
    else {
      memset(buf,0,PAGE_SIZE);
      dbgmsg("Zeroing: 0x%x\n",mem_addr+i);
    }
    if (proc_memmap(pid,mem_addr+i,mem_getphysaddr(buf),writable,1,0)!=NULL) proc_memunmap(own_pid,buf);
    else {
      proc_memfree(own_pid,buf);
      ret = -1;
      break;
    }
  }
  return ret;
}
#endif

/**
 * Validates ELF header
 *  @param header Pointer to ELF header
 *  @return Whether header is valid
 */
static int elf_validate(elf_header_t *header) {
  if (header->ident[EI_MAG0]!=ELFMAG0 || header->ident[EI_MAG1]!=ELFMAG1 || header->ident[EI_MAG2]!=ELFMAG2 || header->ident[EI_MAG3]!=ELFMAG3) return 0;
  if (header->machine!=EM_386) return 0;
  if (header->ident[EI_CLASS]!=ELFCLASS32) return 0;
  if (header->ident[EI_DATA]!=ELFDATA2LSB) return 0;
  if (header->ident[EI_VERSION]!=header->version) return 0;
  return 1;
}

static int elf_loadseg(pid_t pid,int fh,void *mem_addr,size_t mem_size,size_t file_addr,size_t file_size,int writable) {
  if (mem_addr<(void*)USERSPACE_ADDRESS) return -1;
  size_t i;
  int ret = 0;
  pid_t own_pid = getpid();

  lseek(fh,file_addr,SEEK_SET);

  for (i=0;i<mem_size;i+=PAGE_SIZE) {
    void *buf = mem_alloc(PAGE_SIZE);
    size_t cur_count = i>file_size?0:(i+PAGE_SIZE>file_size?file_size%PAGE_SIZE:PAGE_SIZE);
    if (cur_count>0) read(fh,buf,cur_count);
    else memset(buf,0,PAGE_SIZE);
    if (proc_memmap(pid,mem_addr+i,mem_getphysaddr(buf),writable,1,0)!=NULL) proc_memunmap(own_pid,buf);
    else {
      proc_memfree(own_pid,buf);
      ret = -1;
      break;
    }
  }
  return ret;
}

elf_t *elf_create(const char *file) {
  int fh = open(file,O_RDONLY);
  if (fh!=-1) {
    elf_header_t header;

    // load header
    read(fh,&header,sizeof(header));
    if (!elf_validate(&header)) return NULL;

    // create elf
    elf_t *elf = malloc(sizeof(elf_t));
    memcpy(&(elf->header),&header,sizeof(header));
    elf->fh = fh;

    return elf;
  }
  else return NULL;
}

void elf_destroy(elf_t *elf) {
  close(elf->fh);
  free(elf);
}

void *elf_load(elf_t *elf,pid_t pid) {
  size_t i;
  elf_progheader_t progheader;

  // Program Headers
  for (i=0;i<elf->header.phnum;i++) {
    lseek(elf->fh,elf->header.phoff+i*sizeof(elf_progheader_t),SEEK_SET);
    read(elf->fh,&progheader,sizeof(elf_progheader_t));

    /** @todo remove
    dbgmsg("[Program Header %d]\n",i);
    dbgmsg("ph.type   = 0x%x\n",progheader.type);
    dbgmsg("ph.offset = 0x%x\n",progheader.offset);
    dbgmsg("ph.vaddr  = 0x%x\n",progheader.vaddr);
    dbgmsg("ph.paddr  = 0x%x\n",progheader.paddr);
    dbgmsg("ph.filesz = 0x%x\n",progheader.filesz);
    dbgmsg("ph.memsz  = 0x%x\n",progheader.memsz);
    dbgmsg("ph.flags  = 0x%x\n",progheader.flags);
    dbgmsg("ph.align  = 0x%x\n\n",progheader.align); */

    if (progheader.type==PT_LOAD) {
      if (progheader.align!=PAGE_SIZE) return NULL;
      if (elf_loadseg(pid,elf->fh,(void*)(progheader.vaddr),progheader.memsz,progheader.offset,progheader.filesz,(progheader.flags&PF_W)==PF_W)==-1) return NULL;
    }
  }

  return (void*)(elf->header.entry);
}
