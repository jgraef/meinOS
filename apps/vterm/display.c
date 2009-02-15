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
#include <ringbuf.h>
#include <stdlib.h>

#include "vterm.h"

int vt_display_init() {
  vt_displays = llist_create();

  return 0;
}

void vt_display_clear(vt_display_t *display) {
  memset(display->buffer,0,display->mode.w*display->mode.h*display->mode.bpx);
}

vt_display_t *vt_display_create(void *buffer) {
  vt_display_t *display = malloc(sizeof(vt_display_t));
  display->buffer = buffer;
  /// hack
  display->mode.w = 80;
  display->mode.h = 25;
  display->mode.bpx = 2;
  display->mode.type = VT_GMODE_TEXT;

  llist_push(vt_displays,display);
  vt_display_clear(display);
  return display;
}

void vt_display_destroy(vt_display_t *display) {
  llist_remove(vt_displays,llist_find(vt_displays,display));
  free(display);
}
