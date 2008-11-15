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
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>

struct _malloc_header;
struct _malloc_header {
  uint32_t flags;
  size_t size; /// size of block; includes sizeof(struct _malloc_header)
  struct _malloc_header *nextfree; /// pointer to next free block
};

struct _malloc_header *_malloc_firstfree;
void *(*_malloc_getpage)(size_t size);
void (*_malloc_freepage)(void *addr);

/**
 * Initializes memory allocator
 *  @param getpage Pointer to page allocator
 *  @param freepage Pointer to page free'r
 */
void malloc_init(void *(*getpage)(size_t size),void (*freepage)(void *addr)) {
  _malloc_getpage = getpage;
  _malloc_freepage = freepage;
  _malloc_firstfree = NULL;
}

/**
 * Insert a new free block in free block list
 *  @param header Pointer to new block
 */
void _malloc_insertlist(struct _malloc_header *header) {
  struct _malloc_header *cur = _malloc_firstfree;

  if (_malloc_firstfree==NULL) {
    _malloc_firstfree = header;
    header->nextfree = NULL;
  }
  else if (header<(struct _malloc_header*)_malloc_firstfree) {
    header->nextfree = _malloc_firstfree;
    _malloc_firstfree = header;
  }
  else {
    while (cur!=NULL) {
      if (cur<header && (cur->nextfree>header || cur->nextfree==NULL)) {
        if (((uint32_t)cur)+cur->size==(uint32_t)header) { ///< join last block and new one
          cur->size += header->size;
        }
        else {
          header->nextfree = cur->nextfree;
          cur->nextfree = header;
        }
        break;
      }
      cur = cur->nextfree;
    }
  }

  if (((uint32_t)header)+header->size==((uint32_t)header->nextfree)) { ///< join next block and new one
    header->size += header->nextfree->size;
    header->nextfree = header->nextfree->nextfree;
  }
}

/**
 * Donates memory for a free block
 *  @param addr Address of free memory
 *  @param size Size of free memory
 */
void malloc_donatemem(void *addr,size_t size) {
  if (addr!=NULL) {
    struct _malloc_header *header = (struct _malloc_header*)addr;
    header->flags = 0;
    header->size = size;
    _malloc_insertlist(header);
  }
}

/**
 * Adds a new free block of the size of a page to list
 *  @return Address of new memory block
 */
void *_malloc_getmem() {
  void *addr = _malloc_getpage(PAGE_SIZE);
  malloc_donatemem(addr,PAGE_SIZE);
  return addr;
}

/**
 * Gets a free block and marks it as used
 *  @param size Needed block's size
 *  @return Pointer to block
 */
struct _malloc_header *_malloc_getfree(size_t size) {
  while (1) {
    struct _malloc_header *cur = _malloc_firstfree;
    struct _malloc_header *last = NULL;

    while (cur!=NULL) {
      //if (size>=cur->size && size<=cur->size+sizeof(struct _malloc_header)) { ///< I want this block!
      if (size==cur->size) {
        cur->flags |= 1;
        if (last==NULL) _malloc_firstfree = cur->nextfree;
        else last->nextfree = cur->nextfree;
        return cur;
      }
      else if (cur->size>=size+sizeof(struct _malloc_header)) { ///< Just split it
        struct _malloc_header *new = (struct _malloc_header*)(((uint8_t*)cur)+size);
        new->flags = 0;
        new->size = cur->size-size;
        new->nextfree = cur->nextfree;
        cur->size = size;
        cur->flags |= 1;
        if (last==NULL) _malloc_firstfree = new;
        else last->nextfree = new;
        return cur;
      }
      last = cur;
      cur = cur->nextfree;
    }
    _malloc_getmem();
  }
}

/**
 * Checks for a free page and frees it
 */
void _malloc_checkforfree() {
  struct _malloc_header *cur = _malloc_firstfree;
  struct _malloc_header *last = NULL;

  while (cur!=NULL) {
    if (cur->size==PAGE_SIZE && ((uint32_t)cur)%PAGE_SIZE==0) { ///< We can just free it
      if (last==NULL) _malloc_firstfree = cur->nextfree;
      else last->nextfree = cur->nextfree;
      _malloc_freepage(cur);
    }
    else if (cur->size-(((uint32_t)cur)%PAGE_SIZE)>=PAGE_SIZE && (((uint32_t)cur)%PAGE_SIZE)>=sizeof(struct _malloc_header)) { ///< Split it and free it
      cur->size -= PAGE_SIZE;
      /**
       * Die If-Abfrage gilt im Moment nur wenn der Block an einer Pagegrenze aufhört.
       * Der Block muss (manchmal) in _mehrere_ (2) Blöcke gespalten werden. Der 1te muss
       * dann auch auf den 2ten zeigen.
       */
       _malloc_freepage(0/** @todo **/);
    }
    last = cur;
    cur = cur->nextfree;
  }
}

/**
 * Allocates memory
 *  @param size How many bytes
 *  @return Pointer to allocated memory
 */
void *malloc(size_t size) {
  struct _malloc_header *mem = _malloc_getfree(size+sizeof(struct _malloc_header));
  if (mem==NULL) return NULL;
  else return mem+1;
}

/**
 * Frees memory
 *  @param address Pointer to memory to free
 *  @todo free free pages
 */
void free(void* address) {
  struct _malloc_header *header = ((struct _malloc_header*)address)-1;
  header->flags = 0; ///< mark block as free
  _malloc_insertlist(header);

  //_malloc_checkforfree();
}

/**
 * Reallocates block
 *  @param address Address of block
 *  @param size New block size
 *  @return Pointer to new block
 */
void* realloc(void* address, size_t size) {
  struct _malloc_header *header = ((struct _malloc_header*)address)-1;
  if (size==0) {
    free(address);
    address = NULL;
  }
  else if (size<header->size) { // make block smaller (split)
    if (header->size-size<sizeof(struct _malloc_header)); /// @todo what to do?
    else {
      struct _malloc_header *new = (struct _malloc_header*)(((uint8_t*)header)+size);
      new->flags = 0;
      new->size = header->size-size;
      _malloc_insertlist(new);
      header->size = size;
    }
  }
  else if (size>header->size) {
    struct _malloc_header *cur = _malloc_firstfree;
    while (cur!=NULL) {
      if (cur==header+header->size && cur->size>=size-header->size) { // use next block
        if (cur->size-(size-header->size)<sizeof(struct _malloc_header)); /// @todo what to do?
        else {
          struct _malloc_header *new = (struct _malloc_header*)(((uint8_t*)cur)+size-header->size);
          new->flags = 0;
          new->size = cur->size-(size-header->size);
          _malloc_insertlist(new);
          header->size = size;
        }
        break;
      }
      cur = cur->nextfree;
    }
    if (header->size!=size) { // no space: malloc, memcpy, free
      void *new = malloc(size);
      memcpy(new,address,header->size);
      free(address);
      address = new;
    }
  }
  return address;
}

/**
 * Allocates memory for an array of n elements where each element is elsize bytes large.
 *  @param n Number of elements
 *  @param elsize Size of each element
 */
void* calloc(size_t n,size_t elsize) {
  if (n==0 || elsize==0) return NULL;
  void *mem = malloc(n*elsize);
  memset(mem,0,n*elsize);
  return mem;
}
