#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <sys/types.h>
#include <syscalls.h>

int syscall_call(int syscall,size_t numparams,...);

#endif
