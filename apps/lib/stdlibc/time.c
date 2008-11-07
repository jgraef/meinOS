#include <sys/types.h>
#include <time.h>
#include <cmos.h>

static time_t start_time = 0;
static clock_t start_ticks;

time_t time(time_t *tloc) {
  if (start_time==0) {
    struct tm tm = {
      .tm_sec = cmos_getsecond(),
      .tm_min = cmos_getminute(),
      .tm_hour = cmos_gethour(),
      .tm_mday = cmos_getday(),
      .tm_mon = cmos_getmonth()-1,
      .tm_year = cmos_getyear()-1900
    };

    start_time = mktime(&tm);
    start_ticks = clock();
  }

  time_t time = start_time+(clock()-start_ticks)/CLOCKS_PER_SEC;

  if (tloc!=NULL) *tloc = time;
  return time;
}

time_t mktime(struct tm *tm) {
  return 0;
}

static clock_t start_clock = -1;
clock_t clock() {
  if (start_clock==-1) start_clock = syscall_call(SYSCALL_TIME_GETTICKS,0);
  return syscall_call(SYSCALL_TIME_GETTICKS,0)-start_clock;
}
