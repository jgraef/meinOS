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

#ifndef _CDI_MISC_H_
#define _CDI_MISC_H_

#include <sys/types.h>
#include <cdi.h>
#include <irq.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

static inline void cdi_register_irq(uint8_t irq,void (*func)(struct cdi_device *dev),struct cdi_device *dev) {
  irq_reghandler(irq,func,dev,0);
}

static inline void cdi_sleep_ms(uint32_t ms) {
  if (ms<1000) usleep(ms*1000);
  else sleep(ms/1000);
}

static inline uint64_t cdi_time() {
  return (uint64_t)time(NULL);
}

static inline uint64_t cdi_time_offset(unsigned int hour,unsigned int minute,unsigned int second) {
  return hour*3600+minute*60+second;
}

int cdi_ioports_alloc(uint16_t start,uint16_t count);
int cdi_ioports_free(uint16_t start,uint16_t count);
uint64_t cdi_time_by_date(unsigned int year,unsigned int month,unsigned int day);
void* cdi_alloc_phys_addr(size_t size,uintptr_t paddr);
void cdi_free_phys_addr(size_t size,uintptr_t paddr);

#endif
