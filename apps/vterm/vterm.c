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
#include <llist.h>

void vt_init() {
  memset(vt_shortcuts,0,sizeof(vt_shortcuts));
}

vt_term_t *vt_term_create(int shortcut) {
  vt_term_t *term = malloc(sizeof(vterm_t));
  if (shortcut>=0 && shortcut<10) vt_shortcuts[shortcut] = term;
  llist_push(vt_terminals,term);
  return term;
}

void vt_term_destroy(vt_term_t *term) {
  llist_remove(vt_terminals,llist_find(vt_terminals,term));
  free(term);
}

