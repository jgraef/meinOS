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
#include <irq.h>
#include <ioport.h>
#include <wchar.h>

#include "vterm.h"

#define VT_KEYBOARD_IRQ                 1
#define VT_KEYBOARD_IOPORT_DATA         0x60
#define VT_KEYBOARD_IOPORT_STATUS       0x64
#define VT_KEYBOARD_BUFSZ               128
#define VT_KEYBOARD_DEFAULT_LAYOUT      "/boot/etc/keyboard_layouts/de"

#define VT_KEYBOARD_IS_SHIFT(scancode)  ((scancode)==0x2A || (scancode)==0x36)
#define VT_KEYBOARD_IS_ALTGR(scancode)  ((scancode)==0xB8)
#define VT_KEYBOARD_IS_CTRL(scancode)   ((scancode)==0x1D || (scancode)==0x9D)
#define VT_KEYBOARD_IS_ALT(scancode)    ((scancode)==0x38 || (scancode)==0xBD)
#define VT_KEYBOARD_IS_FXX(scancode)    (((scancode)>=0x3B && (scancode)<0x45) || (scancode)==0x54 || (scancode)==0x55)

static __inline__ wchar_t *vt_keyboard_load_layout(const char *path);
static void vt_keyboard_irqhandler(vt_keyboard_t *keyboard);

int vt_keyboard_init() {
  if (ioport_reg(VT_KEYBOARD_IOPORT_DATA)==-1) return -1;
  if (ioport_reg(VT_KEYBOARD_IOPORT_STATUS)==-1) return -1;
  irq_reghandler(VT_KEYBOARD_IRQ,vt_keyboard_irqhandler,&vt_keyboard,0);
  vt_keyboard.buffer = ringbuf_create(VT_KEYBOARD_BUFSZ);
  vt_keyboard.layout = vt_keyboard_load_layout(VT_KEYBOARD_DEFAULT_LAYOUT);
  vt_keyboard_irqhandler(&vt_keyboard);

  return 0;
}

static __inline__ wchar_t *vt_keyboard_load_layout(const char *path) {
  FILE *fd = fopen(path,"r");
  if (fd!=NULL) {
    char sig[19];
    fread(sig,1,19,fd);
    if (memcmp(sig,"meinOS-KL",9)==0) {
      void *buf = malloc(0x400*sizeof(wchar_t));
      fread(buf,sizeof(wchar_t),0x400,fd);
      fclose(fd);
      return buf;
    }
    fclose(fd);
  }
  return NULL;
}

static int vt_keyboard_get_fxx(int scancode) {
  if (scancode>=0x3B && scancode<0x45) return scancode-0x3A;
  else if (scancode==0x54) return 11;
  else if (scancode==0x55) return 12;
  else return 0;
}

static wchar_t vt_keyboard_scancode_to_wchar(vt_keyboard_t *keyboard,int scancode) {
  if (keyboard->layout==NULL) return 0;

  int modifier = (keyboard->modifiers.shift?1:0)|(keyboard->modifiers.altgr?2:0);
  return keyboard->layout[scancode*4+modifier];
}

static void vt_keyboard_irqhandler(vt_keyboard_t *keyboard) {
  while (inb(VT_KEYBOARD_IOPORT_STATUS)&1) {
    int released,scancode;

    // get scancode
    scancode = inb(VT_KEYBOARD_IOPORT_DATA);
    //DEBUG("scancode: 0x%x\n",scancode);
    if (scancode!=0xE0) {
      released = scancode&0x80;
      scancode &= 0x7F;
    }

    // check for escaped key
    if (keyboard->escape) {
      scancode |= 0x80;
      keyboard->escape = 0;
    }
    if (scancode==0xE0) {
      keyboard->escape = 1;
      continue;
    }

    // if shift is pressed/released
    if (VT_KEYBOARD_IS_SHIFT(scancode)) keyboard->modifiers.shift = !released;
    // if AltGr is pressed/released
    else if (VT_KEYBOARD_IS_ALTGR(scancode)) keyboard->modifiers.altgr = !released;
    // if Shift is pressed/released
    else if (VT_KEYBOARD_IS_ALTGR(scancode)) keyboard->modifiers.altgr = !released;
    // if Alt is pressed/released
    else if (VT_KEYBOARD_IS_ALT(scancode)) keyboard->modifiers.alt = !released;
    // if Fxx is pressed
    else if (VT_KEYBOARD_IS_FXX(scancode) && keyboard->modifiers.ctrl && keyboard->modifiers.alt) {
      int fxx = vt_keyboard_get_fxx(scancode);
      if (vt_shortcuts[fxx]>0) vt_term_activate(vt_shortcuts[fxx-1]);
    }
    // normal key pressed
    else if (!released) {
      wchar_t chr = vt_keyboard_scancode_to_wchar(keyboard,scancode);
      if (chr>0x7F) DEBUG("TODO: Wide characters (%s:%d): 0x%x\n",__FILE__,__LINE__,chr);
      else if (chr!=0) {
        //DEBUG("keyboard: Read character: '%c' (0x%02x)\n",chr,chr);
        // escape sequence
        if (keyboard->modifiers.ctrl) {
          if (chr=='@') chr = 0x00;
          else if (chr>='A' && chr<='Z') chr = chr-'A'+1;
          else if (chr=='[') chr = 0x1B;
          else if (chr=='\\') chr = 0x1C;
          else if (chr==']') chr = 0x1D;
          else if (chr=='^') chr = 0x1E;
          else if (chr=='_') chr = 0x1F;
        }
        // normal character
        else {
          ringbuf_write(keyboard->buffer,&chr,1);
          dbgmsg("%c",chr);
        }
      }
    }
  }
}
