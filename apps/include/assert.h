#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sys/cdefs.h>

#ifdef NDEBUG
  #define assert(expr) ((void)0)
#else
  void __assert_fail(const char *assertion,const char *file,unsigned int line,const char *function);
  #define assert(expr) (expr?0:__assert_fail(__STRING(expr),__FILE__,__LINE__,__func__))
#endif

#endif

