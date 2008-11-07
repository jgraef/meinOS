#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdint.h>

/// Videotext Address
#define VGA_TEXT_ADDRESS       0xB8000
/// 80 cols (chars per line)
#define VGA_TEXT_WIDTH         80
/// 25 rows (lines)
#define VGA_TEXT_HEIGHT        25
/// 2 bytes per char
#define VGA_TEXT_BPC           sizeof(vga_text_char_t)
/// Size of VGA text buffer
#define VGA_TEXT_SIZE          (VGA_TEXT_WIDTH*VGA_TEXT_HEIGHT*VGA_TEXT_BPC)
/// Default color
#define VGA_TEXT_COLOR_DEFAULT VGA_TEXT_COLOR_GRAY_BLACK
/// Tabsize
#define VGA_TEXT_TABSIZE       8

typedef enum {
  VGA_TEXT_COLOR_GREEN_BLACK = 0x02,
  VGA_TEXT_COLOR_GRAY_BLACK = 0x07,
  VGA_TEXT_COLOR_RED_BLACK = 0x0C,
  VGA_TEXT_COLOR_WHITE_BLUE = 0x1F
} vga_text_color_t;

typedef struct {
  /// X position on screen
  int x;
  /// Y position on screen
  int y;
  /// Color of next char
  vga_text_color_t color;
} vga_text_cursor_t;

typedef struct {
  uint8_t chr;
  uint8_t attr;
} vga_text_char_t;

vga_text_char_t *vga_text_video;
vga_text_cursor_t vga_text_cursor;

void vga_init();
void vga_text_clear();
void vga_text_set_hwcursor(int x,int y);
void vga_text_setcolor(vga_text_color_t color);
void vga_text_printchar(char chr);

/// @todo remove
void com_init();
void com_send(char a);

#endif
