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

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#define NULL ((void*)0)

typedef unsigned int       size_t;
typedef signed int         ssize_t;
typedef signed int         blkcnt_t;
typedef signed long long   blkcnt64_t;
typedef ssize_t            blksize_t;
typedef unsigned int       clock_t;
typedef unsigned int       clockid_t;
typedef unsigned int       dev_t;
typedef unsigned int       fsblkcnt_t;
typedef size_t             fsfilcnt_t;
typedef signed int         id_t;
typedef id_t               gid_t;
typedef unsigned int       ino_t;
typedef unsigned long long ino64_t;
typedef long               key_t;
typedef unsigned int       mode_t;
typedef unsigned int       nlink_t;
typedef int                off_t;
typedef long long          off64_t;
typedef id_t               pid_t;
typedef signed int         suseconds_t;
typedef unsigned int       time_t;
typedef id_t               uid_t;
typedef unsigned int       useconds_t;

#define __USE_BSD /* for bash */

#ifdef __USE_BSD
  #ifndef __u_char_defined
    #define __u_char_defined
    typedef unsigned char u_char;
    typedef unsigned short u_short;
    typedef unsigned int u_int;
    typedef unsigned long u_long;
  #endif
#endif

#endif
