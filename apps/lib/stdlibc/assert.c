#include <stdio.h>
#include <misc.h>
#include <unistd.h>
#include <stdlib.h>

void __assert_fail(const char *assertion,const char *file,unsigned int line,const char *function) {
  char *name = getname(getpid());
  printf("%s: %s:%u: %s: Assertion `%s' failed\n",name,file,line,function,assertion);
  free(name);
  abort();
}
