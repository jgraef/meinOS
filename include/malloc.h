#ifndef _MALLOC_H_
#define _MALLOC_H_

// This header declares malloc functions for kernel and userspace

void *malloc(size_t size);
void free(void* address);
void *realloc(void* address,size_t size);
void *calloc(size_t n,size_t elsize);

#endif
