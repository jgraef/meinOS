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

#include <lapic.h>
#include <stdint.h>
#include <pic.h>
#include <interrupt.h>
#include <paging.h>
#include <cpu.h>
#include <isr.h>
#include <debug.h>
#include <memkernel.h>

/**
 * Initializes LAPIC
 *  @return 0=Success; -1=Failure
 */
#include <stddef.h>
int lapic_init() {
  uint32_t edx = 0;
  cpu_t *cpu = cpu_this;

  pic_init();

  cpu_id(1,NULL,NULL,NULL,&edx);
  if (edx&(1<<9) && 0) {
    isr_uselapic = 1;
    cpu->uselapic = 1;

    lapic = memkernel_findvirt(1)+PAGEOFF(LAPIC_PHYS_ADDRESS);
    if (paging_map(PAGEDOWN(lapic),PAGEDOWN(LAPIC_PHYS_ADDRESS),0,1)<0) panic("Cannot map LAPIC\n");

    lapic->tpr = 0x20;
    lapic->lvt_timer   = 0x20030;
    lapic->lvt_thermal = 0x20031;
    lapic->lvt_pmc     = 0x20032;
    lapic->lvt_lint0   = 0x08700;
    lapic->lvt_lint1   = 0x08700;
    lapic->lvt_error   = 0x20035;
    lapic->spurious    = 0x0010F;

    pic_pit_setinterval(0,LAPIC_PIT_CALIBRATE_INTERVAL);

    return 0;
  }
  else {
    isr_uselapic = 0;
    cpu->uselapic = 0;
    cpu->interval = 10; // IRQ0 all 10 ms
    pic_pit_setinterval(0,cpu->interval);
    return -1;
  }
}

/**
 * Sends EOI
 */
void lapic_eoi() {
  lapic->eoi = 0;
}

void lapic_timer_calibrate() {
  static unsigned int ticks = 0;
  static unsigned int start = 0;
  static unsigned int end = 0;
  kprintf("LAPIC timer calibration: ticks=%u; start=%u; end=%u\n",ticks,start,end);
}