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

#ifndef _DEBUG_H_
#define _DEBUG_H_

//#define NODEBUG

#ifndef NOEBUG
  #include <kprint.h>
  #include <sys/cdefs.h>

  #define test(func) { kprintf("%s...",__STRING(func)); kprintf("%s\n",(func)==0?"done":"failed"); }
  #define debug(str) kprintf("%s\n",str!=0?str:"DEBUG")
  #define todo(str) { kprintf("TODO: %s: %d: %s(): %s\n",__FILE__,__LINE__,__func__,str); while (1) cpu_halt(); }
#else
  #define test(func) (func)
  #define debug(str)
  #define todo(str)
#endif

#endif
