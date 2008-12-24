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
#include <devfs.h>
#include <ioport.h>
#include <string.h>
#include <irq.h>
#include <fcntl.h>
#include <wchar.h>
#include <errno.h>

#include "console.h"

#define KEYBOARD_IRQ         0x01
#define KEYBOARD_PORT_DATA   0x60
#define KEYBOARD_PORT_STATUS 0x64
#define KEYBUF_SIZE          128

/// @todo later "/etc/..." should work (symlink)
#define KEYBOARD_DEFAULT_LAYOUT "/boot/etc/keyboard_layouts/de"

#define IS_SHIFT(scancode) (scancode==0x2A || scancode==0x36)
#define IS_ALTCAP(scancode) (scancode==0xB8)

struct {
  int released;
  int escape;
  int shift;
  int altcap;
  struct {
    char *buffer;
    size_t wpos;
    size_t rpos;
    size_t size;
  } buffer;
  struct {
    int has_shift;
    int has_altcap;
    unsigned int width;  // Num of attributes
    unsigned int height; // Num of keys
    wchar_t *table;
  } layout;
} keyboard;

static int keyboard_layout_load(const char *filename) {
  int fh = open(filename,O_RDONLY);
  if (fh!=-1) {
    read(fh,&(keyboard.layout.has_shift),1);
    read(fh,&(keyboard.layout.has_altcap),1);
    keyboard.layout.width = 1+(keyboard.layout.has_shift?1:0)+(keyboard.layout.has_altcap?1:0);
    read(fh,&(keyboard.layout.height),4);
    keyboard.layout.table = calloc(keyboard.layout.width*keyboard.layout.height,sizeof(wchar_t));
    read(fh,keyboard.layout.table,keyboard.layout.width*keyboard.layout.height*sizeof(wchar_t));
    DEBUG("keyboard: Layout loaded: %s (width=%d; height=%d; shift=%s; altcap=%s)\n",filename,keyboard.layout.width,keyboard.layout.height,keyboard.layout.has_shift?"yes":"no",keyboard.layout.has_altcap?"yes":"no");
    close(fh);
    return 0;
  }
  else {
    DEBUG("keyboard: %s: %s\n",strerror(errno),filename);
    return -1;
  }
}

/*static void keyboard_layout_unload() {
  free(keyboard.layout.table);
  memset(&keyboard.layout,0,sizeof(keyboard.layout));
}*/

static wchar_t scancode2wchr(int scancode) {
  if (keyboard.layout.table==NULL) return 0;
  // find right column in table
  int col = 0;
  if (keyboard.shift && keyboard.layout.has_shift) col = 1;
  else if (keyboard.altcap && keyboard.layout.has_altcap) col = 2;
  //if (col>=keyboard.layout.width) col = keyboard.layout.width-1;
  // find right row in table
  int row = scancode;
  if (row>=keyboard.layout.height) row = keyboard.layout.height-1;
  // return character
  return keyboard.layout.table[row*keyboard.layout.width+col];
}

/**
 * Gets keys from keyboard and parses it to characters
 *  @param buffer Buffer for read chars
 *  @param count How many chars to read
 *  @param dev Device to read from
 *  @return How many bytes read
 */
ssize_t onread(devfs_dev_t *dev,void *buffer,size_t count,off_t offset) {
  count = count>keyboard.buffer.wpos-keyboard.buffer.rpos?keyboard.buffer.wpos-keyboard.buffer.rpos:count;
  if (count>0) {
    memcpy(buffer,keyboard.buffer.buffer+keyboard.buffer.rpos,count);
    keyboard.buffer.rpos += count;
  }
  return count;
}

/**
 * Handles Keyboard IRQ
 *  @param null Unused
 *  @todo Caps lock?
 */
static void keyboard_irq(void *null) {
  int released,scancode;

  while (inb(KEYBOARD_PORT_STATUS)&1) {
    // get scancode
    scancode = inb(KEYBOARD_PORT_DATA);
    if (scancode!=0xE0) {
      released = scancode&0x80;
      scancode &= 0x7F;
    }

    // check for escaped key
    if (keyboard.escape) {
      scancode |= 0x80;
      keyboard.escape = 0;
    }
    if (scancode==0xE0) {
      keyboard.escape = 1;
      continue;
    }

    // if shift is pressed/released
    if (IS_SHIFT(scancode)) keyboard.shift = !released;
    // if alternative capital is pressed/released
    else if (IS_ALTCAP(scancode)) keyboard.altcap = !released;

    // write key into buffer
    else if (!released) {
      wchar_t chr = scancode2wchr(scancode);
      if (chr>0x7F) DEBUG("TODO: Wide characters (%s %d)\n",__FILE__,__LINE__);
      else if (chr!=0) {
        //DEBUG("keyboard: Read character: '%c' (0x%02x)\n",chr,chr);
        keyboard.buffer.buffer[keyboard.buffer.wpos++] = chr;
      }
    }
  }
}

/**
 * Initializes keyboard driver
 *  @return 0=success; -1=failure
 */
int init_keyboard() {
  if (ioport_reg(KEYBOARD_PORT_DATA)==-1) return -1;
  if (ioport_reg(KEYBOARD_PORT_STATUS)==-1) return -1;
  irq_reghandler(KEYBOARD_IRQ,keyboard_irq,NULL,0);

  memset(&keyboard,0,sizeof(keyboard));
  keyboard.buffer.size = KEYBUF_SIZE;
  keyboard.buffer.buffer = malloc(keyboard.buffer.size);
  keyboard_layout_load(KEYBOARD_DEFAULT_LAYOUT);

  keyboard_irq(NULL);
  return 0;
}
