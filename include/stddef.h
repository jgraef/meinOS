#ifndef _STDDEF_H_
#define _STDDEF_H_

#include <sys/types.h>

#ifndef NULL
  #define NULL ((void*)0)
#endif

#ifndef offsetof
  #define offsetof(struct_type, member) ((size_t)&(((struct_type*)0)->member))
#endif

typedef int wchar_t;

#endif
