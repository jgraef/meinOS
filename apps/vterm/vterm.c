#include <sys/types.h>
#include <llist.h>

vterm_t *vterm_create(int shortcut,int did) {
  static int next_vterm_id = 0;
  vterm_t *vterm = malloc(sizeof(vterm_t));
  vterm->vid = next_vterm_id++;
  vterm->shortcut = shortcut;
  llist_push(vterms,vterm);
}

void vterm_destroy(vterm_t *vterm) {
  llist_destroy(vterms,llist_find(vterms,vterm));
  free(vterm);
}

