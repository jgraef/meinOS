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

#ifndef _ELF_H_
#define _ELF_H_

#include <sys/types.h>
#include <stdint.h>
#include <memuser.h>

#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff
// Elf Header Machine
#define EM_NONE  0
#define EM_M32   1
#define EM_SPARC 2
#define EM_386   3
#define EM_68K   4
#define EM_88K   5
#define EM_860   7
#define EM_MIPS  8
// Elf Header Version
#define EV_NONE 0
#define EV_CURRENT 1
// Elf Header ident
#define EI_MAG0          0
#define EI_MAG1          1
#define EI_MAG2          2
#define EI_MAG3          3
#define EI_CLASS         4
#define EI_DATA          5
#define EI_VERSION       6
#define EI_PAD           7
#define EI_NIDENT       16
#define ELFMAG0       0x7f
#define ELFMAG1        'E'
#define ELFMAG2        'L'
#define ELFMAG3        'F'
#define ELFCLASSNONE     0
#define ELFCLASS32       1
#define ELFCLASS64       2
#define ELFDATANONE      0
#define ELFDATA2LSB      1
#define ELFDATA2MSB      2

// Programm Header Type
#define PT_NULL         0x00000000
#define PT_LOAD         0x00000001
#define PT_DYNAMIC      0x00000002
#define PT_INTERP       0x00000003
#define PT_NOTE         0x00000004
#define PT_SHLIB        0x00000005
#define PT_PHDR         0x00000006
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff
// unofficial
#define PT_LOOS         0x60000000
#define PT_HIOS         0x6fffffff
#define PT_GNU_EH_FRAME 0x6474e550
#define PT_GNU_STACK    (PT_LOOS+0x474e551)

// Programm Header Flag
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

typedef struct {
  uint8_t  ident[EI_NIDENT];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint32_t entry;
  uint32_t phoff;
  uint32_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
} elf_header_t;

typedef struct {
  uint32_t type;
  uint32_t offset;
  uint32_t vaddr;
  uint32_t paddr;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags;
  uint32_t align;
} elf_progheader_t;

int elf_validate(elf_header_t *header);
int elf_loadseg(void *memaddr,size_t memsize,void *fileaddr,size_t filesize,addrspace_t *addrspace,int writable);
void *elf_load(addrspace_t *addrspace,void *file,size_t filesize);

#endif
