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

#include <stdio.h>
#include <rpc.h>
#include <misc.h>

#include "vterm.h"

int main(int argc,char *argv[]) {
  if (vt_display_init()==-1) {
    DEBUG("vterm: Error initializing display\n");
    fprintf(stderr,"vterm: Error initializing display\n");
    return 1;
  }
  if (vt_keyboard_init()==-1) {
    DEBUG("vterm: Error initializing keyboard\n");
    fprintf(stderr,"vterm: Error initializing keyboard\n");
    return 1;
  }
  if (vt_term_init()==-1) {
    DEBUG("vterm: Error initializing terminals\n");
    fprintf(stderr,"vterm: Error initializing terminals\n");
    return 1;
  }

  void *vgabuf = mem_getvga();
  vt_display_t *display = vt_display_create(vgabuf);

  /*vt_term_t *term =*/ vt_term_create(0,display,&vt_keyboard);

  rpc_mainloop(-1);

  return 0;
}
