#ifndef _MEMKERNEL_H_
#define _MEMKERNEL_H_

#include <sys/types.h>
#include <malloc.h>

#define kmalloc(size) malloc(size)
#define kfree(addr)   free(addr)

int memkernel_init();
void *memkernel_findvirt(size_t pages);
void *memkernel_alloc(size_t size);
void memkernel_free(void *addr);

#endif
