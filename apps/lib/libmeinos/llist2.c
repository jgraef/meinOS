#include <llist2.h>
#include <sys/types.h>
#include <stdlib.h>

llist2_t *llist2_create(void) {
  llist2_t *list = malloc(sizeof(llist2_t));
  list->size = 0;
  list->first_item = NULL;
  return list;
}

void llist2_destroy(llist2_t *list) {
  if (list==NULL) return;
  llist2_item_t *item = list->first_item;
  while (item!=NULL) {
    llist2_item_t *next = item->next;
    free(item);
    item = next;
  }
  free(list);
}
