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
