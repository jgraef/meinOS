#ifndef _SWAP_H_
#define _SWAP_H_

#include <sys/types.h>
#include <procm.h>

#define SWAP_IN  1
#define SWAP_OUT 2
#define SWAP_REM 3

typedef struct {
  int op;
  pid_t pid;
  void *page;
} swap_call_t;

llist_t swap_queue;
proc_t *swap_proc;
void *swap_buf;

int swap_init();
int swap_enable(void *buf);
int swap_call(int op,proc_t *proc,void *page);
int swap_in(proc_t *proc,void *page);
int swap_out(proc_t *proc,void *page);
int swap_remove(proc_t *proc,void *page);

#endif
