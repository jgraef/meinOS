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

#ifndef _BIOSINT_H_
#define _BIOSINT_H_

#include <stdint.h>
#include <procm.h>

int biosint_init();
uint16_t biosint_call(unsigned int *meminfo,size_t meminfo_count,uint16_t ax,uint16_t bx,uint16_t cx,uint16_t dx,uint16_t si,uint16_t di,uint16_t ds,uint16_t es);

#endif
