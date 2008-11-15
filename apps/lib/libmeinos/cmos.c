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

#include <sys/types.h>
#include <ioport.h>
#include <cmos.h>

/// @todo Do this via RPC

/**
 * Reads value from CMOS
 *  @param offset Offset in CMOS
 *  @return Read Value
 */
int cmos_read(size_t offset) {
  int val;
  outb(CMOS_PORT_ADDRESS,(inb(CMOS_PORT_ADDRESS)&0x80)|(offset&0x7F));
  val = inb(CMOS_PORT_DATA);
  return val;
}

/**
 * Writes value to CMOS
 *  @param offset Offset in CMOS
 *  @param val Value to write
 */
void cmos_write(size_t offset,int val) {
  outb(CMOS_PORT_ADDRESS,(inb(CMOS_PORT_ADDRESS)&0x80)|(offset&0x7F));
  outb(CMOS_PORT_DATA,val);
}
