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

#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

#include <sys/types.h>
#include <stdint.h>

#define MULTIBOOT_MAGIC 0x2BADB002

#define multiboot_checkflag(mbi,flag) (mbi->flags&(1<<flag))

typedef enum {
  MULTIBOOT_FREE = 1,
  MULTIBOOT_RESERVED
} multiboot_mmap_type_t;

typedef struct {
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint8_t  boot_device[4];
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  uint32_t syms[4];
  uint32_t mmap_length;
  uint32_t mmap_addr;
  uint32_t drives_length;
  uint32_t drives_addr;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint32_t vbe_mode;
  uint32_t vbe_interface_seq;
  uint32_t vbe_interface_off;
  uint32_t vbe_interface_len;
} __attribute__ ((packed)) multiboot_info_t;

typedef struct {
  uint32_t size;
  uint64_t base;
  uint64_t length;
  multiboot_mmap_type_t type:32;
} __attribute__ ((packed)) multiboot_mmape_t;

typedef struct {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t mod_name;
  uint32_t reserved;
} __attribute__ ((packed)) multiboot_mod_t;

int multiboot_init(multiboot_info_t *mbi);
size_t multiboot_get_memlower();
size_t multiboot_get_memupper();
int multiboot_get_bootdev();
char* multiboot_get_cmdline();
void *multiboot_get_mod(size_t i,char **name,size_t *size);
char *multiboot_get_bootloader();
int multiboot_get_mmap(int item,void **addr,size_t *length,multiboot_mmap_type_t *type);

#endif
