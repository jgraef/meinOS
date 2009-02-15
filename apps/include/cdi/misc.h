/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
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
int cdi_alloc_phys_mem(size_t size,void **vaddr,void **paddr);
void* cdi_alloc_phys_addr(size_t size,uintptr_t paddr);

#endif
