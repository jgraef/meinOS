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

#ifndef _PIC_H_
#define _PIC_H_

#define PIC1                        0x20     // IO base address for master PIC
#define PIC2                        0xA0     // IO base address for slave PIC
#define PIC1_COMMAND                PIC1
#define PIC1_DATA                   (PIC1+1)
#define PIC2_COMMAND                PIC2
#define PIC2_DATA                   (PIC2+1)
#define PIC_EOI                     0x20     // End - of - interrupt command code
#define ICW1_ICW4                   0x01     // ICW4 (not) needed
#define ICW1_SINGLE                 0x02     // Single (cascade) mode
#define ICW1_INTERVAL4              0x04     // Call address interval 4 (8)
#define ICW1_LEVEL                  0x08     // Level triggered (edge) mode
#define ICW1_INIT                   0x10     // Initialization - required!
#define ICW4_8086                   0x01     // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO                   0x02     // Auto (normal) EOI
#define ICW4_BUF_SLAVE              0x08     // Buffered mode/slave
#define ICW4_BUF_MASTER             0x0C     // Buffered mode/master
#define ICW4_SFNM                   0x10     // Special fully nested (not)

#define PIT_CHANNELS                0x40     // Channel ports of PIT
#define PIT_FREQ                    1193180  // Frequency of PIT

int pic_init();
unsigned int pic_getmask(int irq);
void pic_setmask(int irq,unsigned int mask);
void pic_eoi(int irq);
int pic_pit_setinterval(int channel,unsigned int interval);

#endif
