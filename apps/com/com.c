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

#include <sys/types.h>
#include <ioport.h>
#include <unistd.h>

#include "com.h"

#define COM_DEFAULT_BAUDRATE 9600

#define COM_REG_DATA 0
#define COM_REG_IER 1
#define COM_REG_IIR 2
#define COM_REG_FCR 2
#define COM_REG_LCR 3
#define COM_REG_MCR 4
#define COM_REG_LSR 5

void com_dev_init(struct com_device *dev) {
  int baud = 115200/COM_DEFAULT_BAUDRATE;
  // Keine Parität
  outb(dev->base+COM_REG_IER,0x00); // no interrupts
  outb(dev->base+COM_REG_LCR,0x80); // dlab
  outb(dev->base+COM_REG_DATA,baud); // Baudrate (low)
  outb(dev->base+COM_REG_IER,baud>>8); // Baudrate (high)
  outb(dev->base+COM_REG_LCR,0x03); // 8 Bits, 1 stopbit
  outb(dev->base+COM_REG_FCR,0xC7);
  outb(dev->base+COM_REG_MCR,0x0B);
}

static int is_transmit_empty(struct com_device *dev) {
  return inb(dev->base+COM_REG_LSR)&0x20;
}

static int is_received(struct com_device *dev) {
   return inb(dev->base+COM_REG_LSR)&1;
}

size_t com_dev_recv(struct com_device *dev,const void *vbuf,size_t n) {
  char *buf = (char*)vbuf;
  size_t i;

  for (i=0;i<n;i++) {
    while (!is_received(dev));
    buf[i] = inb(dev->base+COM_REG_DATA);
  }
  return n;
}

size_t com_dev_send(struct com_device *dev,void *vbuf,size_t n) {
  char *buf = (char*)vbuf;
  size_t i;

  for (i=0;i<n;i++) {
    while (!is_transmit_empty(dev)) usleep(10);
    outb(dev->base+COM_REG_DATA,buf[i]);
  }
  return n;
}
