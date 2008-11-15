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

