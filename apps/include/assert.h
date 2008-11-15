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

#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sys/cdefs.h>

#ifdef NDEBUG
  #define assert(expr) ((void)0)
#else
  void __assert_fail(const char *assertion,const char *file,unsigned int line,const char *function);
  #define assert(expr) (expr?0:__assert_fail(__STRING(expr),__FILE__,__LINE__,__func__))
#endif

#endif

