#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <sys/types.h>

struct time_val {
  time_t      tv_sec;
  suseconds_t tv_usec;
};

#endif
