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

#include "vterm.h"
#include "escape.h"

#define arraysize(array) (sizeof(array)/sizeof(array[0]))

static struct {
  const char *seq;
  int dflt[2];
} vt_escape_seq[] = {
  // see http://www.termsys.demon.co.uk/vtansi.htm
  // Device Status
  {"[c"   ,{0,0}},
  {"[5n"  ,{0,0}},
  {"[6n"  ,{0,0}},
  // Cursor
  {"[%;%H",{0,0}},
  {"[%A"  ,{1,0}},
  {"[%B"  ,{1,0}},
  {"[%C"  ,{1,0}},
  {"[%D"  ,{1,0}},
  {"[%;%f",{0,0}},
  {"[s"   ,{0,0}},
  {"[u"   ,{0,0}},
  {"7"    ,{0,0}},
  {"8"    ,{0,0}},
  // Scrolling
  {"[D"   ,{0,0}},
  {"[M"   ,{0,0}},
  // Erasing Text
  {"[K"   ,{0,0}},
  {"[1K"  ,{0,0}},
  {"[2K"  ,{0,0}},
  {"[J"   ,{0,0}},
  {"[J"   ,{0,0}},
  {"[J"   ,{0,0}},
};

size_t vt_escape_decode(vt_term_t *term,char *escape) {
  int argv[2];
  size_t i,j;

  if (escape[0]!=VT_ESCAPE_CHAR) return 1;
  escape++;

  // decode escape sequence
  for (i=0;i<arraysize(vt_escape_seq);i++) {
    int argc = 0;
    for (j=0;vt_escape_seq[i].seq[j];j++) {
      if (vt_escape_seq[i].seq[j]=='%') {
        char *arg_start = escape+j;
        char *arg_end;
        argv[argc] = strtoul(arg_start,&arg_end,10);
        if (argv[argc]==0 && (errno==EINVAL || errno==ERANGE)) argv[argc] = vt_escape_seq[i].dflt[argc];
        j += arg_end-arg_start-1;
        argc++;
      }
      else if (vt_escape_seq[i].seq[j]!=escape[j]) break;
    }
    if (vt_escape_seq[i].seq[j]==0) break;
  }

  // do operation
  switch (i) {
    default:
      return 1;
  }
}
