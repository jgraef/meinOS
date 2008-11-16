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
#include <stdint.h>
#include <wchar.h>

#define HEIGHT     0x100
#define HAS_SHIFT  1
#define HAS_ALTCAP 0

wchar_t KEYBOARD_LAYOUT[0x100][2] = {
/* Scancode * Description    *//* Characters*/
/* 00       * Keyboard error */ { 0   , 0    },
/* 01       * Escape         */ { 0   , 0    },
/* 02       * 1 !            */ { '1' , '!'  },
/* 03       * 2 @            */ { '2' , '@'  },
/* 04       * 3 #            */ { '3' , '#'  },
/* 05       * 4 $            */ { '4' , '$'  },
/* 06       * 5 %            */ { '5' , '%'  },
/* 07       * 6 ^            */ { '6' , '^'  },
/* 08       * 7 &            */ { '7' , '&'  },
/* 09       * 8 *            */ { '8' , '*'  },
/* 0A       * 9 (            */ { '9' , '('  },
/* 0B       * 0 )            */ { '0' , ')'  },
/* 0C       * - _            */ { '-' , '_'  },
/* 0D       * = +            */ { '=' , '+'  },
/* 0E       * Backspace      */ { '\b', '\b' },
/* 0F       * Tab            */ { '\t', '\t' },
/* 10       * q Q            */ { 'q' , 'Q'  },
/* 11       * w W            */ { 'w' , 'W'  },
/* 12       * e E            */ { 'e' , 'E'  },
/* 13       * r R            */ { 'r' , 'R'  },
/* 14       * t T            */ { 't' , 'T'  },
/* 15       * y Y            */ { 'y' , 'Y'  },
/* 16       * u U            */ { 'u' , 'U'  },
/* 17       * i I            */ { 'i' , 'I'  },
/* 18       * o O            */ { 'o' , 'O'  },
/* 19       * p P            */ { 'p' , 'P'  },
/* 1A       * [ {            */ { '[' , '{'  },
/* 1B       * ] }            */ { ']' , '}'  },
/* 1C       * Enter          */ { '\n', '\n' },
/* 1D       * LCtrl          */ {  0  ,  0   },
/* 1E       * a A            */ { 'a' , 'A'  },
/* 1F       * s S            */ { 's' , 'S'  },
/* 20       * d D            */ { 'd' , 'D'  },
/* 21       * f F            */ { 'f' , 'F'  },
/* 22       * g G            */ { 'g' , 'G'  },
/* 23       * h H            */ { 'h' , 'H'  },
/* 24       * j J            */ { 'j' , 'J'  },
/* 25       * k K            */ { 'k' , 'K'  },
/* 26       * l L            */ { 'l' , 'L'  },
/* 27       * ; :            */ { ';' , ':'  },
/* 28       * ' "            */ { '\'', '"'  },
/* 29       * ` ~            */ { '`' , '~'  },
/* 2A       * LShift         */ {  0  ,  0   },
/* 2B       * \ |            */ { '\\', '|'  },
/* 2C       * z Z            */ { 'z' , 'Z'  },
/* 2D       * x X            */ { 'x' , 'X'  },
/* 2E       * c C            */ { 'c' , 'C'  },
/* 2F       * v V            */ { 'v' , 'V'  },
/* 30       * b B            */ { 'b' , 'B'  },
/* 31       * n N            */ { 'n' , 'N'  },
/* 32       * m M            */ { 'm' , 'M'  },
/* 33       * , <            */ { ',' , '<'  },
/* 34       * . >            */ { '.' , '>'  },
/* 35       * / ?            */ { '/' , '?'  },
/* 36       * RShift         */ {  0  ,  0   },
/* 37       * *              */ { '*' , '*'  },
/* 38       * LAlt           */ {  0  ,  0   },
/* 39       * Spacebar       */ { ' ' , ' '  },
/* 3A       * Capslock       */ {  0  ,  0   },
/* 3B       * F1             */ {  0  ,  0   },
/* 3C       * F2             */ {  0  ,  0   },
/* 3D       * F3             */ {  0  ,  0   },
/* 3E       * F4             */ {  0  ,  0   },
/* 3F       * F5             */ {  0  ,  0   },
/* 40       * F6             */ {  0  ,  0   },
/* 41       * F7             */ {  0  ,  0   },
/* 42       * F8             */ {  0  ,  0   },
/* 43       * F9             */ {  0  ,  0   },
/* 44       * F10            */ {  0  ,  0   },
/* 45       * NumLock        */ {  0  ,  0   },
/* 46       * ScrollLock     */ {  0  ,  0   },
/* 47       * Keypad 7       */ { '7' , '7'  },
/* 48       * Keypad 8       */ { '8' , '8'  },
/* 49       * Keypad 9       */ { '9' , '9'  },
/* 4A       * Keypad -       */ { '-' , '-'  },
/* 4B       * Keypad 4       */ { '4' , '4'  },
/* 4C       * Keypad 5       */ { '5' , '5'  },
/* 4D       * Keypad 6       */ { '6' , '6'  },
/* 4E       * Keypad +       */ { '+' , '+'  },
/* 4F       * Keypad 1       */ { '1' , '1'  },
/* 50       * Keypad 2       */ { '2' , '2'  },
/* 51       * Keypad 3       */ { '3' , '3'  },
/* 52       * Keypad 0       */ { '0' , '0'  },
/* 53       * Keypad .       */ { '.' , '.'  },
/* 54       * F11            */ {  0  ,  0   },
/* 55       * F12            */ {  0  ,  0   },
/* 56       *                */ {  0  ,  0   },
/* 57       *                */ {  0  ,  0   },
/* 58       *                */ {  0  ,  0   },
/* 59       *                */ {  0  ,  0   },
/* 5A       *                */ {  0  ,  0   },
/* 5B       *                */ {  0  ,  0   },
/* 5C       *                */ {  0  ,  0   },
/* 5D       *                */ {  0  ,  0   },
/* 5E       *                */ {  0  ,  0   },
/* 5F       *                */ {  0  ,  0   },
/* 60       *                */ {  0  ,  0   },
/* 61       *                */ {  0  ,  0   },
/* 62       *                */ {  0  ,  0   },
/* 63       *                */ {  0  ,  0   },
/* 64       *                */ {  0  ,  0   },
/* 65       *                */ {  0  ,  0   },
/* 66       *                */ {  0  ,  0   },
/* 67       *                */ {  0  ,  0   },
/* 68       *                */ {  0  ,  0   },
/* 69       *                */ {  0  ,  0   },
/* 6A       *                */ {  0  ,  0   },
/* 6B       *                */ {  0  ,  0   },
/* 6C       *                */ {  0  ,  0   },
/* 6D       *                */ {  0  ,  0   },
/* 6E       *                */ {  0  ,  0   },
/* 6F       *                */ {  0  ,  0   },
/* 70       *                */ {  0  ,  0   },
/* 71       *                */ {  0  ,  0   },
/* 72       *                */ {  0  ,  0   },
/* 73       *                */ {  0  ,  0   },
/* 74       *                */ {  0  ,  0   },
/* 75       *                */ {  0  ,  0   },
/* 76       *                */ {  0  ,  0   },
/* 77       *                */ {  0  ,  0   },
/* 78       *                */ {  0  ,  0   },
/* 79       *                */ {  0  ,  0   },
/* 7A       *                */ {  0  ,  0   },
/* 7B       *                */ {  0  ,  0   },
/* 7C       *                */ {  0  ,  0   },
/* 7D       *                */ {  0  ,  0   },
/* 7E       *                */ {  0  ,  0   },
/* 7F       *                */ {  0  ,  0   },
/* 80       *                */ {  0  ,  0   },
/* 81       *                */ {  0  ,  0   },
/* 82       *                */ {  0  ,  0   },
/* 83       *                */ {  0  ,  0   },
/* 84       *                */ {  0  ,  0   },
/* 85       *                */ {  0  ,  0   },
/* 86       *                */ {  0  ,  0   },
/* 87       *                */ {  0  ,  0   },
/* 88       *                */ {  0  ,  0   },
/* 89       *                */ {  0  ,  0   },
/* 8A       *                */ {  0  ,  0   },
/* 8B       *                */ {  0  ,  0   },
/* 8C       *                */ {  0  ,  0   },
/* 8D       *                */ {  0  ,  0   },
/* 8E       *                */ {  0  ,  0   },
/* 8F       *                */ {  0  ,  0   },
/* 90       *                */ {  0  ,  0   },
/* 91       *                */ {  0  ,  0   },
/* 92       *                */ {  0  ,  0   },
/* 93       *                */ {  0  ,  0   },
/* 94       *                */ {  0  ,  0   },
/* 95       *                */ {  0  ,  0   },
/* 96       *                */ {  0  ,  0   },
/* 97       *                */ {  0  ,  0   },
/* 98       *                */ {  0  ,  0   },
/* 99       *                */ {  0  ,  0   },
/* 9A       *                */ {  0  ,  0   },
/* 9B       *                */ {  0  ,  0   },
/* 9C       *                */ {  0  ,  0   },
/* 9D       *                */ {  0  ,  0   },
/* 9E       *                */ {  0  ,  0   },
/* 9F       *                */ {  0  ,  0   },
/* A0       *                */ {  0  ,  0   },
/* A1       *                */ {  0  ,  0   },
/* A2       *                */ {  0  ,  0   },
/* A3       *                */ {  0  ,  0   },
/* A4       *                */ {  0  ,  0   },
/* A5       *                */ {  0  ,  0   },
/* A6       *                */ {  0  ,  0   },
/* A7       *                */ {  0  ,  0   },
/* A8       *                */ {  0  ,  0   },
/* A9       *                */ {  0  ,  0   },
/* AA       *                */ {  0  ,  0   },
/* AB       *                */ {  0  ,  0   },
/* AC       *                */ {  0  ,  0   },
/* AD       *                */ {  0  ,  0   },
/* AE       *                */ {  0  ,  0   },
/* AF       *                */ {  0  ,  0   },
/* B0       *                */ {  0  ,  0   },
/* B1       *                */ {  0  ,  0   },
/* B2       *                */ {  0  ,  0   },
/* B3       *                */ {  0  ,  0   },
/* B4       *                */ {  0  ,  0   },
/* B5       *                */ { '/' , '/'  },
/* B6       *                */ {  0  ,  0   },
/* B7       *                */ {  0  ,  0   },
/* B8       *                */ {  0  ,  0   },
/* B9       *                */ {  0  ,  0   },
/* BA       *                */ {  0  ,  0   },
/* BB       *                */ {  0  ,  0   },
/* BC       *                */ {  0  ,  0   },
/* BD       *                */ {  0  ,  0   },
/* BE       *                */ {  0  ,  0   },
/* BF       *                */ {  0  ,  0   },
/* C0       *                */ {  0  ,  0   },
/* C1       *                */ {  0  ,  0   },
/* C2       *                */ {  0  ,  0   },
/* C3       *                */ {  0  ,  0   },
/* C4       *                */ {  0  ,  0   },
/* C5       *                */ {  0  ,  0   },
/* C6       *                */ {  0  ,  0   },
/* C7       *                */ {  0  ,  0   },
/* C8       *                */ {  0  ,  0   },
/* C9       *                */ {  0  ,  0   },
/* CA       *                */ {  0  ,  0   },
/* CB       *                */ {  0  ,  0   },
/* CC       *                */ {  0  ,  0   },
/* CD       *                */ {  0  ,  0   },
/* CE       *                */ {  0  ,  0   },
/* CF       *                */ {  0  ,  0   },
/* D0       *                */ {  0  ,  0   },
/* D1       *                */ {  0  ,  0   },
/* D2       *                */ {  0  ,  0   },
/* D3       *                */ {  0  ,  0   },
/* D4       *                */ {  0  ,  0   },
/* D5       *                */ {  0  ,  0   },
/* D6       *                */ {  0  ,  0   },
/* D7       *                */ {  0  ,  0   },
/* D8       *                */ {  0  ,  0   },
/* D9       *                */ {  0  ,  0   },
/* DA       *                */ {  0  ,  0   },
/* DB       *                */ {  0  ,  0   },
/* DC       *                */ {  0  ,  0   },
/* DD       *                */ {  0  ,  0   },
/* DE       *                */ {  0  ,  0   },
/* DF       *                */ {  0  ,  0   },
/* E0       *                */ {  0  ,  0   },
/* E1       *                */ {  0  ,  0   },
/* E2       *                */ {  0  ,  0   },
/* E3       *                */ {  0  ,  0   },
/* E4       *                */ {  0  ,  0   },
/* E5       *                */ {  0  ,  0   },
/* E6       *                */ {  0  ,  0   },
/* E7       *                */ {  0  ,  0   },
/* E8       *                */ {  0  ,  0   },
/* E9       *                */ {  0  ,  0   },
/* EA       *                */ {  0  ,  0   },
/* EB       *                */ {  0  ,  0   },
/* EC       *                */ {  0  ,  0   },
/* ED       *                */ {  0  ,  0   },
/* EE       *                */ {  0  ,  0   },
/* EF       *                */ {  0  ,  0   },
/* F0       *                */ {  0  ,  0   },
/* F1       *                */ {  0  ,  0   },
/* F2       *                */ {  0  ,  0   },
/* F3       *                */ {  0  ,  0   },
/* F4       *                */ {  0  ,  0   },
/* F5       *                */ {  0  ,  0   },
/* F6       *                */ {  0  ,  0   },
/* F7       *                */ {  0  ,  0   },
/* F8       *                */ {  0  ,  0   },
/* F9       *                */ {  0  ,  0   },
/* FA       *                */ {  0  ,  0   },
/* FB       *                */ {  0  ,  0   },
/* FC       *                */ {  0  ,  0   },
/* FD       *                */ {  0  ,  0   },
/* FE       *                */ {  0  ,  0   },
/* FF       *                */ {  0  ,  0   },


};

int main() {
  struct {
    uint8_t has_shift;
    uint8_t has_altcap;
    uint32_t height;
  } __attribute__ ((packed)) header = {
    .has_shift = HAS_SHIFT,
    .has_altcap = HAS_ALTCAP,
    .height = HEIGHT
  };

  fwrite(&header,1,sizeof(header),stdout);
  fwrite(KEYBOARD_LAYOUT,1,sizeof(KEYBOARD_LAYOUT),stdout);

  return 0;
}
