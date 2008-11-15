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

#include <pic.h>
#include <ioport.h>

int pic_init() {
  int offset1 = 0x20;
  int offset2 = 0x28;
  char a1,a2;
  a1 = inb(PIC1_DATA);                     // save masks
  a2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND,ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence
  outb(PIC2_COMMAND,ICW1_INIT+ICW1_ICW4);
  outb(PIC1_DATA,offset1);                 // define the PIC vectors
  outb(PIC2_DATA,offset2);
  outb(PIC1_DATA,4);                       // continue initialization sequence
  outb(PIC2_DATA,2);
  outb(PIC1_DATA,ICW4_8086);
  outb(PIC2_DATA,ICW4_8086);

  outb(PIC1_DATA,a1);   // restore saved masks.
  outb(PIC2_DATA,a2);

  return 0;
}

/**
 * Gets IRQ mask
 *  @param irq IRQ
 *  @return IRQ mask
 */
unsigned int pic_getmask(int irq) {
  unsigned int mask = inb(PIC1_DATA);
  mask |= inb(PIC2_DATA)<<8;
  return mask;
}

/**
 * Sets IRQ mask
 *  @param irq IRQ
 *  @param mask IRQ mask
 */
void pic_setmask(int irq,unsigned int mask) {
  outb(PIC1_DATA,mask);
  outb(PIC2_DATA,mask>>8);
}

/**
 * Sends EOI to PIC
 *  @param irq IRQ
 */
void pic_eoi(int irq) {
  if (irq>=8) outb(PIC2_COMMAND,PIC_EOI);
  outb(PIC1_COMMAND,PIC_EOI);
}

/**
 * Sets interval of PIT
 *  @param interval Interval
 *  @param channel Channel
 *  @return 0=Success; -1=Failure
 */
int pic_pit_setinterval(int channel,unsigned int interval) {
  unsigned int val = PIT_FREQ*interval/1000;
  if (channel<4) {
    outb(PIT_CHANNELS+channel,val);
    outb(PIT_CHANNELS+channel,(val>>8));
    return 0;
  }
  else return -1;
}
