/*
 * Copyright (c) 2007 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Kevin Wolf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <llist.h>
#include <malloc.h>

/**
 * Creates a linked list
 *  @return linked list
 */
llist_t llist_create() {
  llist_t list = malloc(sizeof(struct llist_S));
  list->anchor = NULL;
  list->size = 0;
  return list;
}

/**
 * Destroys a linked list
 *  @param list linked list to destroy
 */
void llist_destroy(llist_t list) {
  while (llist_pop(list)!=NULL);
  free(list);
}

/**
 * Gets size of a linked list
 *  @param list linked list to get size of
 *  @return linked list
 */
size_t llist_size(llist_t list) {
  if (!list || !list->anchor) return 0;
  else return list->size;
}

/**
 * Push element to linked list
 *  @param list linked list to get size of
 *  @param value Value to add to list
 *  @return linked list
 */
llist_t llist_push(llist_t list,void *value) {
  if (!list) return NULL;
  struct llist_node *node = malloc(sizeof(struct llist_node));
  node->value = value;
  node->next = list->anchor;
  list->anchor = node;
  list->size++;
  return list;
}

/**
 * Pops element from linked list
 *  @param list linked list to pop from
 *  @return Value popped
 */
void* llist_pop(llist_t list) {
  struct llist_node* old_anchor;
  void* value;

  if (!list) return NULL;

  if (list->anchor) {
    value = list->anchor->value;
    old_anchor = list->anchor;
    list->anchor = list->anchor->next;
    free(old_anchor);
    list->size--;
  }
  else value = NULL;

  return value;
}

/**
 * Gets element from a list
 *  @param list linked list to get element from
 *  @param index Index of element
 *  @return Element
 */
struct llist_node *llist_get_node_at(llist_t list,size_t index) {
  if (!list || !list->anchor || index<0) return NULL;
  struct llist_node* current = list->anchor;
  int n = index;
  if (index>list->size-1) return NULL;

  while (n--) {
    current = current->next;
    if (!current) return NULL;
  }

  return current;
}

/**
 * Gets value of a element in a list
 *  @param list linked list to get value from
 *  @param index Index of element to get value from
 *  @return Value
 */
void *llist_get(llist_t list,size_t index) {
  struct llist_node *node = llist_get_node_at(list,index);
  if (node==NULL) return NULL;
  else return node->value;
}

/**
 * Inserts an element in a list
 *  @param list linked list to insert in
 *  @param index Index of new element
 *  @param value Value of new element
 *  @return linked list
 */
llist_t llist_insert(llist_t list,size_t index,void* value) {
  struct llist_node* new_node = malloc(sizeof(struct llist_node));
  new_node->value = value;

  if (!list) return NULL;

  if (index) {
    struct llist_node* prev_node = llist_get_node_at(list,index-1);
    if (!prev_node) return NULL;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
  }
  else {
    new_node->next = list->anchor;
    list->anchor = new_node;
  }

  list->size++;

  return list;
}

/**
 * Removes element from list
 *  @param list linked list to remove element from
 *  @param index Index of element to remove
 *  @return Value of removed element
 */
void* llist_remove(llist_t list, size_t index) {
    void* value;

    if (!list) return NULL;

    struct llist_node* node;
    if (index) {
      struct llist_node* prev_node = llist_get_node_at(list,index-1);

      if (!prev_node || !prev_node->next) return NULL;

      node = prev_node->next;
      prev_node->next = node->next;
      value = node->value;
      free(node);
    }
    else {
      if (!list->anchor) return NULL;

      node = list->anchor;
      list->anchor = node->next;;
      value = node->value;
      free(node);
    }

    list->size--;

    return value;
}

/**
 * Finds element in list
 *  @param list Linked list to search in
 *  @param find Element to find
 *  @return Index of found Element
 */
size_t llist_find(llist_t list,void *find) {
  void *element;
  size_t i;

  for (i=0;(element = llist_get(list,i));i++) {
    if (element==find) return i;
  }
  return -1;
}

/**
 * Copies list
 *  @param list Linked list to be copied
 *  @return New linked list
 */
llist_t llist_copy(llist_t list) {
  size_t i;
  void *element;
  llist_t new = llist_create();
  for (i=0;(element = llist_get(list,i));i++) llist_push(new,element);
  return new;
}
