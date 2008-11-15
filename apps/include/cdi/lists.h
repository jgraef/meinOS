/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CDI_LISTS
#define _CDI_LISTS

#include <sys/types.h>
#include <llist.h>

typedef llist_t cdi_list_t;

/**
 * Creates a CDI list
 *  @return CDI list
 */
static inline cdi_list_t cdi_list_create() {
  return (cdi_list_t)llist_create();
}

/**
 * Destroys a CDI list
 *  @param list CDI list to destroy
 */
static inline void cdi_list_destroy(cdi_list_t list) {
  llist_destroy((llist_t)list);
}

/**
 * Gets whether CDI list is empty
 *  @param list CDI list to check
 */
static inline size_t cdi_list_empty(cdi_list_t list) {
  return llist_empty((llist_t)list);
}

/**
 * Gets size of a CDI list
 *  @param list CDI list to get size of
 *  @return CDI list
 */
static inline size_t cdi_list_size(cdi_list_t list) {
  return llist_size((llist_t)list);
}

/**
 * Push element to CDI list
 *  @param list CDI list to get size of
 *  @param value Value to add to list
 *  @return CDI list
 */
static inline cdi_list_t cdi_list_push(cdi_list_t list,void *value) {
  return (cdi_list_t)llist_push((llist_t)list,value);
}

/**
 * Pops element from CDI list
 *  @param list CDI list to pop from
 *  @return Value popped
 */
static inline void* cdi_list_pop(cdi_list_t list) {
  return llist_pop((llist_t)list);
}

/**
 * Gets value of a element in a list
 *  @param list CDI list to get value from
 *  @param index Index of element to get value from
 *  @return Value
 */
static inline void *cdi_list_get(cdi_list_t list,size_t index) {
  return llist_get((llist_t)list,index);
}

/**
 * Inserts an element in a list
 *  @param list CDI list to insert in
 *  @param index Index of new element
 *  @param value Value of new element
 *  @return CDI list
 */
static inline cdi_list_t cdi_list_insert(cdi_list_t list,size_t index,void* value) {
  return (cdi_list_t)llist_insert((llist_t)list,index,value);
}

/**
 * Removes element from list
 *  @param list CDI list to remove element from
 *  @param index Index of element to remove
 *  @return Value of removed element
 */
static inline void* cdi_list_remove(cdi_list_t list,size_t index) {
  return llist_remove((llist_t)list,index);
}


#endif
