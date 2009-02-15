/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <strings.h>
#include <stddef.h>

int strcasecmp(const char *str1,const char *str2) {
  size_t i;
  for (i=0;(str1[i]!=0 && str2[i]!=0);i++) {
    if (str1[i]!=str2[i]) return str1[i]-str2[i];
  }
  return 0;
}

int strncasecmp(const char *str1,const char *str2,size_t n) {
  size_t i;
  for (i=0;(str1[i]!=0 && str2[i]!=0 && i<n);i++) {
    if (str1[i]!=str2[i]) return str1[i]-str2[i];
  }
  return 0;
}

int ffs(int x) {
  size_t i;
  for (i=0;i<sizeof(int)*8;i++) {
    if (x&(1<<i)) return i+1;
  }
  return 0;
}
