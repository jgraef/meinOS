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

  for (i=0;i<file_size;i+=PAGE_SIZE) {
    void *buf = mem_alloc(PAGE_SIZE);
    read(fh,buf,i*PAGE_SIZE>file_size?PAGE_SIZE:file_size-i*PAGE_SIZE);
    proc_memunmap(getpid(),buf);
    if (proc_memmap(pid,mem_addr+i,mem_getphysaddr(buf),writable,1,0)==-1) {
      ret = -1;
      break;
    }
  }

  return ret;
}

void *elf_load(pid_t pid,const char *file) {
  int fh = open(file,O_RDONLY);
  if (fh!=-1) {
    size_t i;
    void *entrypoint;
    elf_header_t header;
    elf_progheader_t *progheader;

    // Header
    read(fh,&header,sizeof(header));
    elf_validate(&header);
    entrypoint = (void*)(header.entry);

    // Program Headers
    progheader = malloc(sizeof(elf_progheader_t)*header.phnum);
    lseek(fh,header.phoff,SEEK_SET);
    read(fh,&progheader,sizeof(elf_progheader_t)*header.phnum);
    for (i=0;i<header.phnum;i++) {
      if (progheader[i].type==PT_LOAD) {
        if (elf_loadseg(pid,fh,(void*)(progheader[i].vaddr),progheader[i].memsz,progheader[i].offset,progheader[i].filesz,(progheader[i].flags&PF_W)==PF_W)==-1) {
          entrypoint = NULL;
          break;
        }
      }
    }

    free(progheader);
    close(fh);
    return entrypoint;
  }
  else return NULL;
}
