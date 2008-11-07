#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <stdint.h>

uintmax_t strntoumax(const char *nptr, char **endptr, int base, size_t n);

static inline intmax_t strntoimax(const char *nptr,char **endptr,int base,size_t n) {
  return (intmax_t) strntoumax(nptr,endptr,base,n);
}

static inline uintmax_t strtoumax(const char *nptr,char **endptr,int base) {
  return (uintmax_t)strntoumax(nptr,endptr,base,~(size_t)0);
}

static inline intmax_t strtoimax(const char *nptr,char **endptr,int base) {
  return (intmax_t)strntoumax(nptr,endptr,base,~(size_t)0);
}

#endif
