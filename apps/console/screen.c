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

#include <stdlib.h>
#include <stdint.h>
#include <devfs.h>
#include <string.h>
#include <misc.h>
#include <unistd.h>
#include <ioport.h>
#include <ctype.h>

#include "console.h"

#define VIDEOTEXT_WIDTH    80      // 80 Cols (chars per line)
#define VIDEOTEXT_HEIGHT   25      // 25 Rows (lines)
#define VIDEOTEXT_SIZE     4000    // Video size
#define VIDEOTEXT_STDCOLOR 0x07    // Default color: black background, lightgray foreground
#define VIDEOTEXT_STDCOL   0       // Default col
#define VIDEOTEXT_STDROW   0       // Default row

#define SCREEN_DEFAULT_FONT "standard"

#define cursor_offset() (cursor.row*VIDEOTEXT_WIDTH+cursor.col)

// see http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
// for changing fonts

static struct {
  int col;
  int row;
} cursor,saved_cursor;

static struct {
  int linewrap;
  int bell_freq;
  int bell_dur;
  int color;
} settings;

static uint16_t *videomem;

/**
 * Rings the bell
 *  @param freq Frequency
 *  @param dur Duration time
 */
static void bell(int freq,int dur) {
  freq = 1193180/freq;
  outb(0x43,0xB6);
  outb(0x42,(uint8_t)freq);
  outb(0x42,(uint8_t)(freq>>8));
  outb(0x61,inb(0x61)|3);
  sleep(dur);
  outb(0x61,inb(0x61)&~3);
}

/**
 * Updates hardware cursor
 */
static void update_hwcursor() {
  uint32_t pos = cursor.row*VIDEOTEXT_WIDTH+cursor.col;
  outb(0x3D4,15);
  outb(0x3D5,pos);
  outb(0x3D4,14);
  outb(0x3D5,pos>>8);
}

/**
 * Clears screen
 */
static void clearscreen() {
  size_t i;
  for (i=0;i<VIDEOTEXT_WIDTH*VIDEOTEXT_HEIGHT;i++) videomem[i] = VIDEOTEXT_STDCOLOR<<8;
  cursor.col = VIDEOTEXT_STDCOL;
  cursor.row = VIDEOTEXT_STDROW;
  update_hwcursor();
}

/**
 * Sets default settings
 */
static void default_settings() {
  settings.linewrap = 1;
  settings.bell_freq = 440;
  settings.bell_dur = 100;
  settings.color = VIDEOTEXT_STDCOLOR;
}

/**
 * Prints a character on screen
 *  @param chr Character
 */
static int printchar(char chr) {
  size_t pos = cursor_offset();
  static int escape = 0;
  static char escape_buf[32];

  if (escape) {
    escape_buf[(escape++)-1] = chr; // put byte in escape buffer

    if (isalpha(chr)) { // escape code finished
      if (escape==1 && escape_buf[0]=='c') default_settings();           // reset device
      else if (escape==3 && memcmp(escape_buf,"[7h",3)==0) settings.linewrap = 1; // enable linewrap
      else if (escape==3 && memcmp(escape_buf,"[7l",3)==0) settings.linewrap = 0; // disable linewrap
      else if (escape_buf[0]=='[' && (escape_buf[escape-2]=='H' || escape_buf[escape-2]=='f')) { // set cursor position
        int row = -1;
        int col = -1;
        if (escape>3) sscanf(escape_buf,escape_buf[escape-2]=='H'?"[%d;%dH":"[%d;%df",&row,&col);
        if (row!=-1 && col!=-1) {
          cursor.col = col;
          cursor.row = row;
        }
        else {
          cursor.col = 0;
          cursor.row = 0;
        }
      }
      else if (escape_buf[0]=='[' && escape_buf[escape-2]=='A') {
        int up = 1;
        if (escape>3) sscanf(escape_buf,"[%dA",&up);
        if (cursor.row>0) cursor.row -= up;
      }
      else if (escape_buf[0]=='[' && escape_buf[escape-2]=='B') {
        int down = 1;
        if (escape>3) sscanf(escape_buf,"[%dB",&down);
        if (cursor.row<VIDEOTEXT_HEIGHT) cursor.row += down;
      }
      else if (escape_buf[0]=='[' && escape_buf[escape-2]=='C') {
        int left = 1;
        if (escape>3) sscanf(escape_buf,"[%dC",&left);
        if (cursor.col>0) cursor.col -= left;
      }
      else if (escape_buf[0]=='[' && escape_buf[escape-2]=='D') {
        int right = 1;
        if (escape>3) sscanf(escape_buf,"[%dD",&right);
        if (cursor.col<VIDEOTEXT_WIDTH) cursor.col += right;
      }
      else if (escape_buf[0]=='[' && (escape_buf[1]=='s' || escape_buf[1]=='7')) {
        memcpy(&saved_cursor,&cursor,sizeof(cursor));
      }
      else if (escape_buf[0]=='[' && (escape_buf[1]=='u' || escape_buf[1]=='8')) {
        memcpy(&cursor,&saved_cursor,sizeof(cursor));
      }
      else if (memcmp(escape_buf,"[K",2)==0) {
        memset(videomem+pos,0,(VIDEOTEXT_WIDTH-cursor.col+1)*2);
      }
      else if (memcmp(escape_buf,"[1K",3)==0) {
        memset(videomem+VIDEOTEXT_WIDTH*cursor.col,0,cursor.row*2);
      }
      else if (memcmp(escape_buf,"[2K",3)==0) {
        memset(videomem+VIDEOTEXT_WIDTH*cursor.col,0,VIDEOTEXT_WIDTH);
      }
      else if (memcmp(escape_buf,"[J",2)==0) {
        memset(videomem+VIDEOTEXT_WIDTH*cursor.col,0,(VIDEOTEXT_HEIGHT-cursor.row)*VIDEOTEXT_WIDTH*2);
      }
      else if (memcmp(escape_buf,"[1J",3)==0) {
        memset(videomem,0,cursor.row*VIDEOTEXT_WIDTH*2);
      }
      else if (memcmp(escape_buf,"[2J",3)==0) {
        clearscreen();
      }
      escape = 0;
    }
  }
  else if (chr=='\a') bell(settings.bell_freq,settings.bell_dur);
  else if (chr=='\b') cursor.col = (cursor.col-1>0)?(cursor.col-1):0;
  else if (chr=='\t') cursor.col = cursor.col+5;
  else if (chr=='\n') {
    cursor.row++;
    cursor.col = VIDEOTEXT_STDCOL;
  }
  else if (chr=='\f') clearscreen();
  else if (chr=='\r') cursor.col = VIDEOTEXT_STDCOL;
  else if (chr==0x1B) escape = 1;
  else if (chr>=' ') {
    *(videomem+pos) = (((uint16_t)settings.color)<<8)|chr;
    cursor.col++;
  }

  if (cursor.col>=VIDEOTEXT_WIDTH) {
    cursor.col = cursor.col-VIDEOTEXT_WIDTH;
    cursor.row++;
  }
  while (cursor.row>=VIDEOTEXT_HEIGHT) {
    memcpy(videomem,videomem+VIDEOTEXT_WIDTH,VIDEOTEXT_SIZE-VIDEOTEXT_WIDTH*2);
    memset(videomem+VIDEOTEXT_WIDTH*(VIDEOTEXT_HEIGHT-1),0,VIDEOTEXT_WIDTH*2);
    cursor.row--;
  }

  update_hwcursor();

  return 1;
}

/**
 * Writes characters to screen
 *  @param buffer Chars to write to screen
 *  @param count How many bytes to write to screen
 *  @param dev Device to write to
 *  @return How many bytes written
 */
ssize_t onwrite(devfs_dev_t *dev,void *buffer,size_t count,off_t offset) {
  int i;
  int w = 0;
  for (i=0;i<count;i++) w += printchar(((char*)buffer)[i]);
  return w;
}

/**
 * Initializes screen driver
 *  @return 0=success; -1=failure
 */
int init_screen() {
  videomem = mem_getvga();
  if (videomem==NULL) return -1;
  memset(&saved_cursor,0,sizeof(saved_cursor));
  default_settings();
  clearscreen();
  return 0;
}
