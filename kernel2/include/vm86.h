/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
