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

#ifndef _APIC_H_
#define _APIC_H_

#include <stdint.h>

#define LAPIC_PHYS_ADDRESS 0xFEE00000

#define LAPIC_REG_ID            0x020
#define LAPIC_REG_VERSION       0x030
#define LAPIC_REG_TPR           0x080
#define LAPIC_REG_APR           0x090
#define LAPIC_REG_PPR           0x0A0
#define LAPIC_REG_EOI           0x0B0
#define LAPIC_REG_LOGDEST       0x0D0
#define LAPIC_REG_DESTFMT       0x0E0
#define LAPIC_REG_SPURIOUS      0x0F0
#define LAPIC_REG_ERROR         0x280
#define LAPIC_REG_LVT_TIMER     0x320
#define LAPIC_REG_LVT_THERMAL   0x330
#define LAPIC_REG_LVT_PERFOR    0x340
#define LAPIC_REG_LVT_LINT0     0x350
#define LAPIC_REG_LVT_LINT1     0x360
#define LAPIC_REG_LVT_ERROR     0x370
#define LAPIC_REG_TIMER_INITIAL 0x380
#define LAPIC_REG_TIMER_CURRENT 0x390
#define LAPIC_REG_TIMER_DEVIDE  0x3E0

#define lapic_reg(off) (*((uint32_t*)(lapic+(off))))

void *lapic;

int lapic_init();
void lapic_eoi();
void lapic_timer_init();

#endif
