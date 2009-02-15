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
#include <devfs.h>

#include "vterm.h"
#include "escape.h"

#define VT_TERM_SCREENBUF_HEIGHT 100
#define VT_TERM_TABSIZE          8
#define VT_TERM_OUTBUF_SIZE      128

int vt_term_init() {
  vt_terminals = llist_create();
  memset(vt_shortcuts,0,sizeof(vt_shortcuts));

  return 0;
}

static void vt_term_clearscreen(vt_term_t *term) {
  memset(term->screenbuf.buf,0,term->screenbuf.height*term->display->mode.w);
  term->screenbuf.curline = 0;
  term->cursor.x = 0;
  term->cursor.y = 0;
}

static void vt_term_refresh(vt_term_t *term) {
  memcpy(term->display->buffer,term->screenbuf.buf,term->display->mode.w*term->display->mode.h*term->display->mode.bpx);
}

static void vt_term_printchar(vt_term_t *term,char chr) {
  /*if (chr=='\a') ;
  else*/ if (chr=='\b') term->cursor.x = (term->cursor.x>1)?(term->cursor.x-1):0;
  else if (chr=='\t') term->cursor.x = (term->cursor.x/term->tabsize+1)*term->tabsize;
  else if (chr=='\n') {
    term->cursor.y++;
    term->cursor.x = 0;
  }
  else if (chr=='\f') vt_term_clearscreen(term);
  else if (chr=='\r') term->cursor.x = 0;
  else if (chr>=' ') {
    size_t pos = term->cursor.y*term->display->mode.w+term->cursor.x;
    term->screenbuf.buf[pos] = (((uint16_t)term->color)<<8)|chr;
    term->cursor.x++;
  }

  if (term->cursor.x>=term->display->mode.w) {
    term->cursor.x = 0;
    term->cursor.y++;
  }
  if (term->cursor.y>=term->screenbuf.height) {
    /*memcpy(videomem,videomem+VIDEOTEXT_WIDTH,VIDEOTEXT_SIZE-VIDEOTEXT_WIDTH*2);
    memset(videomem+VIDEOTEXT_WIDTH*(VIDEOTEXT_HEIGHT-1),0,VIDEOTEXT_WIDTH*2);*/
    term->cursor.y--;
  }
}

static vt_term_t *vt_term_find_by_dev(devfs_dev_t *dev) {
  vt_term_t *term;
  size_t i;
  for (i=0;(term = llist_get(vt_terminals,i));i++) {
    if (term->dev==dev) return term;
  }
  return NULL;
}

static ssize_t vt_term_onread(devfs_dev_t *dev,void *buf,size_t size,off_t offset) {
  vt_term_t *term = vt_term_find_by_dev(dev);

  if (term==NULL) return 0;

  // read from outbuf
  size_t outbuf_size = ringbuf_read(term->outbuf,buf,size);
  // read from keyboard buffer
  return vt_keyboard_read(term->keyboard,buf+outbuf_size,size-outbuf_size)+outbuf_size;
}

static ssize_t vt_term_onwrite(devfs_dev_t *dev,void *vbuf,size_t size,off_t offset) {
  char *buf = (char*)vbuf;
  vt_term_t *term = vt_term_find_by_dev(dev);
  size_t i;

  if (term==NULL) return 0;

  for (i=0;i<size;i++) {
    if (buf[i]==VT_ESCAPE_CHAR) size += vt_escape_decode(term,buf+i)-1;
    else if (buf[i]&0x80) {
      DEBUG("TODO: %s:%d: print unicode (utf-8): 0x%x\n",__FILE__,__LINE__,buf[i]);
      break;
    }
    else vt_term_printchar(term,buf[i]);
  }

  vt_term_refresh(term);

  return i;
}

vt_term_t *vt_term_create(int shortcut,vt_display_t *display,vt_keyboard_t *keyboard) {
  static int termid = 0;
  char *devname;

  if (display->mode.type!=VT_GMODE_TEXT) return NULL;

  vt_term_t *term = malloc(sizeof(vt_term_t));
  term->display = display;
  term->keyboard = keyboard;
  term->screenbuf.height = VT_TERM_SCREENBUF_HEIGHT;
  term->screenbuf.curline = 0;
  term->screenbuf.buf = malloc(term->screenbuf.height*display->mode.w);
  term->tabsize = VT_TERM_TABSIZE;
  term->outbuf = ringbuf_create(VT_TERM_OUTBUF_SIZE);
  asprintf(&devname,"tty%d",termid++);
  term->dev = devfs_createdev(devname);
  devfs_onread(term->dev,vt_term_onread);
  devfs_onwrite(term->dev,vt_term_onwrite);
  free(devname);
  if (shortcut>=0 && shortcut<10) vt_shortcuts[shortcut] = term;
  llist_push(vt_terminals,term);
  return term;
}

void vt_term_destroy(vt_term_t *term) {
  free(term->screenbuf.buf);
  devfs_removedev(term->dev);
  ringbuf_destroy(term->outbuf);
  llist_remove(vt_terminals,llist_find(vt_terminals,term));
  free(term);
}

void vt_term_activate(vt_term_t *term) {
  vt_curterm = term;
  vt_term_refresh(term);
}
