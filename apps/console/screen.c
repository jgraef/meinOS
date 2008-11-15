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

#include "console.h"

#define VIDEOTEXT_WIDTH    80      // 80 Cols (chars per line)
#define VIDEOTEXT_HEIGHT   25      // 25 Rows (lines)
#define VIDEOTEXT_SIZE     4000    // Video size
#define VIDEOTEXT_STDCOLOR 0x07    // Default color: black background, lightgray foreground
#define VIDEOTEXT_STDCOL   0       // Default col
#define VIDEOTEXT_STDROW   0       // Default row
#define BELL_STDFREQ       440     // Tone A1 (440Hz)
#define BELL_STDDUR        100     // 100 msec

#define SCREEN_DEFAULT_FONT "standard"

#define cursor_offset() (cursor.row*VIDEOTEXT_WIDTH+cursor.col)

// see http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
// for changing fonts

struct {
  int col;
  int row;
  int color;
} cursor;

uint16_t *videomem;

/**
 * Rings the bell
 *  @param freq Frequency
 *  @param dur Duration time
 */
void bell(int freq,int dur) {
  freq = 1193180/freq;
  outb(0x43,0xB6);
  outb(0x42,(uint8_t)freq);
  outb(0x42,(uint8_t)(freq>>8));
  outb(0x61,inb(0x61)|3);
  sleep(dur);
  outb(0x61,inb(0x61)&~3);
}

/**
 * Sets cursor by offset
 *  @param off Offset
 */
void cursor_setbyoffset(int off) {
  cursor.col = off%VIDEOTEXT_WIDTH;
  cursor.row = off/VIDEOTEXT_WIDTH;
}

/**
 * Clears screen
 */
void clearscreen() {
  memset(videomem,0,VIDEOTEXT_SIZE);
  cursor.col = VIDEOTEXT_STDCOL;
  cursor.row = VIDEOTEXT_STDROW;
  cursor.color = VIDEOTEXT_STDCOLOR;
}

/**
 * Prints a character on screen
 *  @param chr Character
 */
int printchar(char chr) {
  int pos;
  if (chr=='\a') bell(BELL_STDFREQ,BELL_STDDUR);
  if (chr=='\b') cursor.col = (cursor.col-1>0)?(cursor.col-1):0;
  if (chr=='\t') cursor.col = cursor.col+5;
  if (chr=='\n') {
    cursor.row++;
    cursor.col = VIDEOTEXT_STDCOL;
  }
  if (chr=='\f') clearscreen();
  if (chr=='\r') cursor.col = VIDEOTEXT_STDCOL;
  if (chr>=' ') {
    pos = cursor_offset();
    *(videomem+pos) = (((uint16_t)cursor.color)<<8)|chr;
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
  clearscreen();
  return 0;
}
