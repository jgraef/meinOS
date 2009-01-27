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

#ifndef _VTERM_H_
#define _VTERM_H_

#include <sys/types.h>
#include <llist.h>

// Display
typedef struct {
  unsigned int x,y,w,h;
  unsigned int bpx; // Bits per Pixel/Character
  enum {
    GMODE_TEXT,
    GMODE_GRAPHIC
  } type;
} vt_gmode_t;

typedef struct {
  void *buffer;
  vt_gmode_t mode;
} vt_display_t;

llist_t vt_displays;

// Keyboard
typedef struct {
  
} vt_keyboard_t;

// VTerm
typedef struct {
  vt_display_t *display;
} vt_term_t;

vt_term_t vt_shortcuts[10];
llist_t vt_terminals;
vt_term_t *vt_curterm;

#endif
