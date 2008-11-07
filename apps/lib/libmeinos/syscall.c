#include <sys/types.h>
#include <stdarg.h>
#include <syscall.h>

int syscall_call(int syscall,size_t numparams,...) {
  va_list args;
  size_t i;
  int ret;

  va_start(args,numparams);
  for (i=0;i<numparams;i++) asm("push %0"::"r"(va_arg(args,int)));
  va_end(args);
  asm("push %0"::"r"(syscall));
  asm("int %1":"=r"(ret):"i"(SYSCALL_INT));
  asm("add %0,%%esp"::"a"((numparams+1)*4));

  return ret;
}
