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
