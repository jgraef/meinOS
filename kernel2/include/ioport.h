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

#include <stdint.h>

#define IOPORT_ISREG(iopb,port) ((*(((int*)iopb)+(port/sizeof(int))))&(1<<(port%sizeof(int))))
#define IOPORT_REG(iopb,port)   (*(((int*)iopb)+(port/sizeof(int))) |= 1<<(port%sizeof(int)))
#define IOPORT_UNREG(iopb,port) (*(((int*)iopb)+(port/sizeof(int))) &= ~(1<<(port%sizeof(int))))

#define IOPORT_MAXNUM 0x10000

int *iopb_general;

static inline uint8_t inb(uint16_t _port) {
  uint8_t _result;
  __asm__("inb %1, %0":"=a"(_result):"Nd"(_port));
  return _result;
}

static inline uint16_t inw(uint16_t _port) {
  uint16_t _result;
  __asm__("inw %1, %0":"=a"(_result):"Nd"(_port));
  return _result;
}

static inline uint32_t inl(uint16_t _port) {
  uint32_t _result;
  __asm__("inl %1, %0":"=a"(_result):"Nd"(_port));
  return _result;
}

static inline void outw(uint16_t _port,uint16_t _data) {
  __asm__("outw %0, %1"::"a"(_data),"Nd"(_port));
}

static inline void outb(uint16_t _port,uint8_t _data) {
  __asm__("outb %0, %1"::"a"(_data),"Nd"(_port));
}

static inline void outl(uint16_t _port,uint32_t _data) {
  __asm__("outl %0, %1"::"a"(_data),"Nd" (_port));
}

int ioport_init();
int ioport_reg(unsigned int port);
int ioport_unreg(unsigned int port);
