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
#include <sizes.h>
#include <stddef.h>
#include <debug.h>
#include <multiboot.h>

/// Pointer to multiboot structure
multiboot_info_t *multiboot_info;

/**
 * Initializes Multiboot
 *  @param mbi Multiboot sturcture
 *  @return 0=Success; -1=Failure
 */
int multiboot_init(multiboot_info_t *mbi) {
  multiboot_info = mbi;
  return 0;
}

/**
 * Gets amount of lower memory
 *  @return Amount of lower memory
 */
size_t multiboot_get_memlower() {
  return ((size_t)multiboot_info->mem_lower)*KBYTES;
}

/**
 * Gets amount of upper memory
 *  @return Amount of upper memory
 */
size_t multiboot_get_memupper() {
  return ((size_t)multiboot_info->mem_upper)*KBYTES;
}

/**
 * Gets bootdevice
 *  @return Bootdevice
 */
int multiboot_get_bootdev() {
  return *((int*)(multiboot_info->boot_device));
}

/**
 * Gets command line
 *  @return Command line
 */
char* multiboot_get_cmdline() {
  return (char*)(multiboot_info->cmdline);
}

/**
 * Gets list of modules
 *  @param i Number of Module
 *  @param name Reference for name pointer
 *  @param size Reference for module size
 */
void *multiboot_get_mod(size_t i,char **name,size_t *size) {
  if (i>=multiboot_info->mods_count) return NULL;
  multiboot_mod_t *mod = ((multiboot_mod_t*)(multiboot_info->mods_addr))+i;
  if (name!=NULL) *name = (char*)(mod->mod_name);
  if (size!=NULL) *size = mod->mod_end-mod->mod_start;
  return (void*)(mod->mod_start);
}

/**
 * Gets name of bootloader
 *  @return Name of bootloader
 */
char *multiboot_get_bootloader() {
  return (char*)(multiboot_info->boot_loader_name);
}

/**
 * Gets a memory map item
 *  @param item Number of item
 *  @param addr Reference for memory block's base address
 *  @param length Reference for memory block's length
 *  @param type Reference for memory block's type
 *  @return 0=Success; -1=Failure
 */
int multiboot_get_mmap(int item,void **addr,size_t *length,multiboot_mmap_type_t *type) {
  if (!multiboot_checkflag(multiboot_info,6)) return -1;
  multiboot_mmape_t *mmap;
  size_t i = 0;

  for (mmap=(multiboot_mmape_t*)multiboot_info->mmap_addr;(unsigned long)mmap<multiboot_info->mmap_addr+multiboot_info->mmap_length;mmap=(multiboot_mmape_t*)((unsigned long)mmap+mmap->size+sizeof(mmap->size))) {
    if (i==item) {
      if (addr!=NULL) *addr = (void*)((unsigned int)mmap->base);
      if (length!=NULL) *length = mmap->length;
      if (type!=NULL) *type = mmap->type;
      return 0;
    }
    i++;
  }

  return -1;
}
