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

void* cdi_alloc_phys_addr(size_t size,uintptr_t paddr) {
  return NULL;
}

void cdi_free_phys_addr(size_t size,uintptr_t paddr) {
}