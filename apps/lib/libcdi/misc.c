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
#include <unistd.h>
#include <ioport.h>
#include <cdi.h>
#include <cdi/misc.h>
#include <stdint.h>

/**
 * Allocates IO ports
 *  @param start First IO port
 *  @param count How many IO ports
 */
int cdi_ioports_alloc(uint16_t start,uint16_t count) {
  uint16_t i;
  for (i=0;i<count;i++) {
    if (ioport_reg(start+i)==-1) return -1;
  }
  return 0;
}

/**
 * Frees IO ports
 *  @param start First IO port
 *  @param count How many IO ports
 */
int cdi_ioports_free(uint16_t start,uint16_t count) {
  uint16_t i;
  for (i=0;i<count;i++) {
    if (!ioport_unreg(start+i)==-1) return -1;
  }
  return 0;
}

static int is_leapyear(unsigned int year) {
  if (year%400==0) return 1;
  else if (year%100==0) return 0;
  else if (year%4==0) return 1;
  else return 0;
}

static int days_per_month(unsigned int month,unsigned int year) {
  unsigned int days_per_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  return days_per_month[month]+(is_leapyear(year)?1:0);
}

/**
 * Calculates time (Seconds since 01.01.1970) by date
 *  @param year Year
 *  @param month Month
 *  @param day Day of month
 */
uint64_t cdi_time_by_date(unsigned int year,unsigned int month,unsigned int day) {
  unsigned int i;
  uint64_t time = 0;

  for (i=1970;i<year;i++) time = (is_leapyear(i)?366:365)*24*3600;
  for (i=0;i<month;i++) time += days_per_month(month,year)*24*3600;
  time += day*3600;

  return time;
}

int cdi_alloc_phys_mem(size_t size,void **vaddr,void **paddr) {
  return -1;
}

void* cdi_alloc_phys_addr(size_t size,uintptr_t _paddr) {
  return NULL;
}
