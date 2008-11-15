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

#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <stdint.h>

uintmax_t strntoumax(const char *nptr, char **endptr, int base, size_t n);

static inline intmax_t strntoimax(const char *nptr,char **endptr,int base,size_t n) {
  return (intmax_t) strntoumax(nptr,endptr,base,n);
}

static inline uintmax_t strtoumax(const char *nptr,char **endptr,int base) {
  return (uintmax_t)strntoumax(nptr,endptr,base,~(size_t)0);
}

static inline intmax_t strtoimax(const char *nptr,char **endptr,int base) {
  return (intmax_t)strntoumax(nptr,endptr,base,~(size_t)0);
}

#endif
