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

#ifndef _TIME_H_
#define _TIME_H_

#include <sys/types.h>

#define CLOCKS_PER_SEC  100 /// @see kernel2/cpu.h
#define CLOCK_REALTIME  0   // ID of Realtime clock (?) ///< @see ?
#define TIMER_ABSTIME   0   // Flag indicating time is absolute ///< @see ?
#define CLOCK_MONOTONIC 1   // ID of monotonic clock (?) ///< @see ?

struct tm {
  int tm_sec;   // Seconds [0..59]
  int tm_min;   // Minute [0..59]
  int tm_hour;  // Hour [0..23]
  int tm_mday;  // Day of month [1..31]
  int tm_mon;   // Month of year [0..11]
  int tm_year;  // Years since 1900
  int tm_wday;  // Day of Week [0..6] (Sunday=0)
  int tm_yday;  // Day of Year [0..365]
  int tm_isdst; // Daylight Savings Flag
};
struct timespec {
  time_t tv_sec;
  long tv_nsec;
};
struct itimerspec {
  struct timespec it_interval; // Timer period.
  struct timespec it_value;    // Timer expiration.
};

time_t time(time_t *tloc);
time_t mktime(struct tm *tm);
char *asctime(const struct tm *timeptr);
clock_t clock();

#endif
