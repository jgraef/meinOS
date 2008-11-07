#ifndef _VM86_H_
#define _VM86_H_

#include <procm.h>
#include <sys/types.h>

#define vm86_create_pagedir() memuser_create_pagedir()

struct vm86_segmentregs {
  uint32_t es;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
};

struct {
  uint32_t *es;
  uint32_t *ds;
  uint32_t *fs;
  uint32_t *gs;
} vm86_curregs;

proc_t *vm86_proc_create(const char *name,uid_t uid,gid_t gid,proc_t *parent);
void vm86_save_segregs(proc_t *proc);
void vm86_load_segregs(proc_t *proc);

#endif
