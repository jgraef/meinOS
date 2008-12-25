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
#include <sys/time.h>
#include <time.h>
#include <syscall.h>
#include <stdio.h>

static clock_t start_clock = -1;

/**
 * Converts date and time to a string
 *  @param timeptr Date and time
 *  @return String
 */
char *asctime(const struct tm *timeptr) {
  char *wday_name[] = {"Sun","Mon","Tue","Wed","Thu", "Fri","Sat"};
  char *mon_name[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  static char result[26];
  sprintf(result,"%s %s %u %02u:%02u:%02u %u\n",wday_name[timeptr->tm_wday],mon_name[timeptr->tm_mon],timeptr->tm_mday,timeptr->tm_hour,timeptr->tm_min, timeptr->tm_sec,1900+timeptr->tm_year);
  return result;
}

/**
 * Builds timestamp from date and time
 *  @param tm Time and date
 *  @return Timestamp
 */
time_t mktime(struct tm *tm) {
  return tm->tm_sec + tm->tm_min*60 + tm->tm_hour*3600 + tm->tm_yday*86400 +
         (tm->tm_year-70)*31536000 + ((tm->tm_year-69)/4)*86400 -
         ((tm->tm_year-1)/100)*86400 + ((tm->tm_year+299)/400)*86400;
}

/**
 * Gets timestamp
 *  @param tloc Reference for timestamp
 *  @return Timestamp
 */
time_t time(time_t *tloc) {
  struct timeval tp;
  gettimeofday(&tp,NULL);
  if (tloc!=NULL) *tloc = tp.tv_sec;
  return tp.tv_sec;
}

/**
 * Returns number of ticks since process start
 *  @return Ticks
 */
clock_t clock() {
  if (start_clock==-1) start_clock = syscall_call(SYSCALL_TIME_GETTICKS,0);
  return syscall_call(SYSCALL_TIME_GETTICKS,0)-start_clock;
}
