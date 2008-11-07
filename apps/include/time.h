#ifndef _TIME_H_
#define _TIME_H_

#include <syscall.h>

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

#define CLOCKS_PER_SEC  100 /// @see kernel2/cpu.h
#define CLOCK_REALTIME  0   // ID of Realtime clock (?) ///< @see ?
#define TIMER_ABSTIME   0   // Flag indicating time is absolute ///< @see ?
#define CLOCK_MONOTONIC 1   // ID of monotonic clock (?) ///< @see ?

time_t time(time_t *tloc);
time_t mktime(struct tm *tm);
clock_t clock();

#endif
