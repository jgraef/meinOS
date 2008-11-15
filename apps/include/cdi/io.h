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

#ifndef _CDI_IO_H_
#define _CDI_IO_H_

#include <ioport.h>

#define cdi_inb(port)      inb(port)
#define cdi_inw(port)      inw(port)
#define cdi_inl(port)      inl(port)
#define cdi_outb(port,val) outb(port,val)
#define cdi_outw(port,val) outw(port,val)
#define cdi_outl(port,val) outl(port,val)

#endif
