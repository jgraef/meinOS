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

#include <string.h>
#include <ctype.h>
#include <ioport.h>
#include <vga.h>

/**
 * Initializes VGA
 */
void vga_init() {
  com_init();
  vga_text_video = (vga_text_char_t*)VGA_TEXT_ADDRESS;
  vga_text_set_hwcursor(0,VGA_TEXT_HEIGHT+1);
  vga_text_clear();
}

/**
 * Clears VGA text screen
 */
void vga_text_clear() {
  vga_text_cursor.x = 0;
  vga_text_cursor.y = 0;
  vga_text_cursor.color = VGA_TEXT_COLOR_DEFAULT;
  // trick: just set whole screen with bg color as bg and front color
  memset(vga_text_video,(vga_text_cursor.color&0xF0)|(vga_text_cursor.color>>4),VGA_TEXT_SIZE);
}

/**
 * Moves VGA text hardware cursor
 *  @param x New X position
 *  @param y New Y position
 */
void vga_text_set_hwcursor(int x,int y) {
  int off = y*VGA_TEXT_WIDTH+x;

  outb(0x3D4,14);
  outb(0x3D5,off>>8);
  outb(0x3D4,15);
  outb(0x3D5,off);
}

/**
 * Set color of next chars
 *  @param color Color
 */
void vga_text_setcolor(vga_text_color_t color) {
  vga_text_cursor.color = color;
}

/**
 * Prints character to VGA text screen
 */
void vga_text_printchar(char chr) {
  if (chr=='\b') vga_text_cursor.x = vga_text_cursor.x>0?vga_text_cursor.x-1:vga_text_cursor.x;
  else if (chr=='\r') vga_text_cursor.x = 0;
  else if (chr=='\n') {
    vga_text_cursor.y++;
    vga_text_cursor.x = 0;
  }
  else if (chr=='\f') vga_text_clear();
  else if (chr=='\t') vga_text_cursor.x = vga_text_cursor.x = vga_text_cursor.x>0?(vga_text_cursor.x/VGA_TEXT_TABSIZE+1)*VGA_TEXT_TABSIZE:0;
  else if (isprint(chr)) {
    int off = vga_text_cursor.x+vga_text_cursor.y*VGA_TEXT_WIDTH;
    if (off==0x3FD) asm("hlt"::"a"(0xD00FC0DE));
    vga_text_video[off].chr = chr;
    vga_text_video[off].attr = vga_text_cursor.color;
    vga_text_cursor.x++;
  }

  com_send(chr);
  bochs_send(chr);

  if (vga_text_cursor.x>=VGA_TEXT_WIDTH) {
    vga_text_cursor.x = 0;
    vga_text_cursor.y++;
  }
  if (vga_text_cursor.y>=VGA_TEXT_HEIGHT) {
    memmove(vga_text_video,vga_text_video+VGA_TEXT_WIDTH,VGA_TEXT_WIDTH*(VGA_TEXT_HEIGHT-1)*VGA_TEXT_BPC);
    memset(vga_text_video+VGA_TEXT_WIDTH*(VGA_TEXT_HEIGHT-1),(vga_text_cursor.color&0xF0)|(vga_text_cursor.color>>4),VGA_TEXT_WIDTH*VGA_TEXT_BPC);
    vga_text_cursor.y = VGA_TEXT_HEIGHT-1;
  }
}

/// @todo remove
#define COM_PORT 0x3f8   /* COM1 */
#define BAUDRATE 9600
void com_init() {
  int baud = 115200/BAUDRATE;
  // Keine Parität
  outb(COM_PORT+1,0x00); // no interrupts
  outb(COM_PORT+3,0x80); // dlab
  outb(COM_PORT+0,baud); // Baudrate (low)
  outb(COM_PORT+1,baud>>8); // Baudrate (high)
  outb(COM_PORT+3,0x03); // 8 Bits, 1 stopbit
  outb(COM_PORT+2,0xC7);
  outb(COM_PORT+4,0x0B);
}
int is_transmit_empty() {
  return inb(COM_PORT+5)&0x20;
}
void com_send(char a) {
  while (!is_transmit_empty());
  outb(COM_PORT,a);
}

#define BOCHS_PORT 0xE9
void bochs_send(char a) {
  outb(BOCHS_PORT,a);
}
