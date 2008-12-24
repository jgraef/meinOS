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
#include <syscall.h>
#include <stdio.h>

static time_t start_time = 0;
static clock_t start_ticks;

static int is_leapyear(unsigned int year) {
  if (year%400==0) return 1;
  else if (year%100==0) return 0;
  else if (year%4==0) return 1;
  else return 0;
}

static void getyday(struct tm *tm) {
  unsigned int mdays[12] = {
    31,
    31+28,
    31+28+31,
    31+28+31+30,
    31+28+31+30+31,
    31+28+31+30+31+30,
    31+28+31+30+31+30+31,
    31+28+31+30+31+30+31+31,
    31+28+31+30+31+30+31+31+30,
    31+28+31+30+31+30+31+31+30+31,
    31+28+31+30+31+30+31+31+30+31+30,
    31+28+31+30+31+30+31+31+30+31+30+31
  };

  tm->tm_yday = (tm->tm_mon>0?mdays[tm->tm_mon-1]:0)+tm->tm_mday;
  if (is_leapyear(tm->tm_year+1900)) tm->tm_yday++;
}

static void getwday(struct tm *tm) {
  unsigned int doomsdays[] = {1,6,4,2};
  unsigned int year = tm->tm_year+1900;
  unsigned int dday_idx = (year/100)%4;
  unsigned int step1 = (year%100)/12;
  unsigned int step2 = (year%100)%12;
  unsigned int step3 = step2/4;
  unsigned int dday_year = (doomsdays[dday_idx]+((step1+step2+step3)%7))%7;
  unsigned int january0 = dday_year+(7-((59+is_leapyear(year)?1:0)%7));
  tm->tm_wday = ((january0+(tm->tm_yday%7))%7)-1;
}

char *asctime(const struct tm *timeptr) {
  char *wday_name[] = {"Sun","Mon","Tue","Wed","Thu", "Fri","Sat"};
  char *mon_name[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  static char result[26];
  sprintf(result,"%s %s %u %02u:%02u:%02u %u\n",wday_name[timeptr->tm_wday],mon_name[timeptr->tm_mon],timeptr->tm_mday,timeptr->tm_hour,timeptr->tm_min, timeptr->tm_sec,1900+timeptr->tm_year);
  return result;
}

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
    getyday(&tm);
    getwday(&tm);

    start_time = mktime(&tm);
    start_ticks = clock();
  }

  time_t time = start_time+(clock()-start_ticks)/CLOCKS_PER_SEC;

  if (tloc!=NULL) *tloc = time;
  return time;
}

time_t mktime(struct tm *tm) {
  return tm->tm_sec + tm->tm_min*60 + tm->tm_hour*3600 + tm->tm_yday*86400 +
         (tm->tm_year-70)*31536000 + ((tm->tm_year-69)/4)*86400 -
         ((tm->tm_year-1)/100)*86400 + ((tm->tm_year+299)/400)*86400;
}

static clock_t start_clock = -1;
clock_t clock() {
  if (start_clock==-1) start_clock = syscall_call(SYSCALL_TIME_GETTICKS,0);
  return syscall_call(SYSCALL_TIME_GETTICKS,0)-start_clock;
}
