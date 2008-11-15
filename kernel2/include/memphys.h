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

#ifndef _MEMPHYS_H_
#define _MEMPHYS_H_

#include <sys/types.h>
#include <limits.h>
#include <sizes.h>

#define MEMPHYS_DMALIMIT        (16*MBYTES)

#define memphys_alloc()    memphys_pop()
#define memphys_free(addr) memphys_push(addr)

/// how much memory available
size_t memphys_memory;

void **memphys_stackbase;
void **memphys_stackend;
void **memphys_stackptr;

int *memphys_dma_bitmap;
size_t memphys_dma_bitmap_size;

int memphys_init();
void *memphys_alloc();
void memphys_free(void *addr);
size_t memphys_memleft();
int memphys_dma_init();
void *memphys_dma_alloc(size_t pages);
void memphys_dma_free(void *addr);

#endif
