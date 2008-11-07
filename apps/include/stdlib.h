#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/types.h>
#include <limits.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#ifndef EXIT_SUCCESS
  #define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
  #define EXIT_FAILURE 1
#endif

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define on_exit(func) atexit(func)

typedef struct {
  int quot;
  int rem;
} div_t;

void exit(int result);
void _Exit(int result);
void abort();
int atexit(void (*function)(void));

int atoi(const char *str);

#define RAND_MAX  32767
int rand_seed;
void srand(int newseed);
int rand();

int   abs(int num);
div_t div(int numer, int denom);

char *getenv(const char *name);
int setenv(const char *envname, const char *envval, int overwrite);
int unsetenv(const char *name);

void *sbrk(intptr_t size);
void* memalign(size_t, size_t);

char *realpath(const char *file_name,char *resolved_name);

static inline signed long strtol(const char *nptr,char **endptr,int base) {
  return (signed long)strntoumax(nptr,endptr,base,~(size_t)0);
}

static inline signed long long strtoll(const char *nptr,char **endptr,int base) {
  return (signed long long)strntoumax(nptr,endptr,base,~(size_t)0);
}

static inline unsigned long strtoul(const char *nptr,char **endptr,int base) {
  return (unsigned long)strntoumax(nptr,endptr,base,~(size_t)0);
}

static inline unsigned long long strtoull(const char *nptr,char **endptr,int base) {
  return (unsigned long long)strntoumax(nptr,endptr,base,~(size_t)0);
}


#endif
