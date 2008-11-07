#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <sys/types.h>
#include <stdint.h>
#include <syscalls.h>
#include <cpu.h>

struct syscall {
  int (*func)();
  int numparams;
};

struct syscall syscalls[SYSCALL_MAXNUM];

int syscall_init();
void syscall_handler(uint32_t *stack);
int syscall_create(int cmd,void *func,int numparams);
void syscall_destroy(int cmd);

#endif
