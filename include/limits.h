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

#ifndef _LIMITS_H_
#define _LIMITS_H_

// only userspace
#define ATEXIT_MAX     (-1)
#define HOST_NAME_MAX  64
#define LOGIN_NAME_MAX 256

// both
#define NAME_MAX       256
#define PATH_MAX       1024

#define SCHAR_MIN      (-128)
#define SCHAR_MAX      127
#define UCHAR_MAX      255
#define CHAR_MAX       SCHAR_MAX
#define CHAR_MIN       SCHAR_MIN
#define CHAR_BIT       8

#define SHRT_MIN       (-32768)
#define SHRT_MAX       32767
#define USHRT_MAX      65535

#define LONG_MAX       0x7FFFFFFF
#define LONG_MIN       (-0x80000000)

#define INT_MAX        0x7FFFFFFF
#define INT_MIN        (-0x80000000)

#define ULONG_MAX      0xFFFFFFFF
#define UINT_MAX       0xFFFFFFFF

#define PAGESIZE       4096
#define PAGE_SIZE      PAGESIZE

// stdio.h
#define FILENAME_MAX   PATH_MAX
#define FOPEN_MAX      (-1)

// sys/uio.h
#define IOV_MAX        4096

#endif
