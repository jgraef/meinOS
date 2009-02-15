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
#include <stdint.h>
#include <llist.h>
#include <ringbuf.h>
#include <wchar.h>
#include <devfs.h>

// Debug
#if 1
  #include <misc.h>
  #define DEBUG(...) do { dbgmsg("vterm: "); \
    dbgmsg(__VA_ARGS__); } while (0)
#else
  #define DEBUG(...)
#endif

// Display (physical)
typedef struct {
  unsigned int w,h;
  unsigned int bpx; // Bytes per Pixel/Character
  enum {
    VT_GMODE_TEXT,
    VT_GMODE_GRAPHIC
  } type;
} vt_gmode_t;

typedef struct {
  void *buffer;
  vt_gmode_t mode;
} vt_display_t;

llist_t vt_displays;

int vt_display_init();
void vt_display_clear(vt_display_t *display);
vt_display_t *vt_display_create(void *buffer);
void vt_display_destroy(vt_display_t *display);

// Keyboard
typedef struct {
  ringbuf_t *buffer;
  int escape; // if received escape scancode
  struct {
    int shift;
    int altgr;
    int ctrl;
    int alt;
  } modifiers;
  wchar_t *layout;
} vt_keyboard_t;

vt_keyboard_t vt_keyboard;

int vt_keyboard_init();
static __inline__ size_t vt_keyboard_read(vt_keyboard_t *keyboard,void *buf,size_t size) {
  return ringbuf_read(keyboard->buffer,buf,size);
}

// VTerm
typedef struct {
  vt_display_t *display;
  vt_keyboard_t *keyboard;
  struct {
    unsigned int x;
    unsigned int y;
  } cursor;
  struct {
    uint16_t *buf;
    unsigned int height;
    unsigned int curline;
  } screenbuf;
  unsigned int tabsize;
  uint8_t color;
  ringbuf_t *outbuf;
  devfs_dev_t *dev;
} vt_term_t;

vt_term_t *vt_shortcuts[12];
llist_t vt_terminals;
vt_term_t *vt_curterm;

int vt_term_init();
vt_term_t *vt_term_create(int shortcut,vt_display_t *display,vt_keyboard_t *keyboard);
void vt_term_destroy(vt_term_t *term);
void vt_term_activate(vt_term_t *term);
static __inline__ size_t vt_term_outbuf_write(vt_term_t *term,void *buf,size_t size) {
  return ringbuf_write(term->outbuf,buf,size);
}

#endif /* _VTERM_H_ */
