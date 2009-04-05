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

#include <libgen.h>
#include <sys/types.h>
#include <proc.h>
#include <stdlib.h>
#include <rpc.h>
#include <misc.h>
#include <errno.h>

#include "init.h"

pid_t proc_fork(void *child_entry) {
  // Create process
  char *name = getname(rpc_curpid);
  pid_t pid = proc_create(name,getuidbypid(rpc_curpid),getgidbypid(rpc_curpid),rpc_curpid);

  if (pid!=-1) {
    // Copy userspace memory
    size_t num_pages,i;
    void **pages = proc_mempagelist(rpc_curpid,&num_pages);

    if (pages!=NULL) {
      for (i=0;i<num_pages;i++) {
        int writable,swappable;
        void *srcp = proc_memget(rpc_curpid,pages[i],NULL,&writable,&swappable,NULL);

        if (srcp!=NULL) {
          // Map parent's page to init
          void *srcv = proc_memmap(1,NULL,srcp,0,0,0);

          // Allocate child's page in init's addrspace
          void *dstv = proc_memmap(1,NULL,NULL,1,0,0);
          void *dstp = mem_getphysaddr(dstv);

          // Map child's page to child
          proc_memmap(pid,pages[i],dstp,writable,swappable,0);

          // Copy
          memcpy(dstv,srcv,PAGE_SIZE);

          proc_memunmap(1,srcv);
          proc_memunmap(1,dstv);
        }
        else proc_memmap(pid,pages[i],NULL,writable,swappable,0);
      }
    }

    // Set stack (we already created one by copying address space)
    int *stack = proc_getstack(rpc_curpid);
    proc_setstack(pid,stack);

    // Set entrypoint of child
    proc_jump(pid,child_entry);
  }

  return pid;
}

int proc_exec(const char *file,int var) {
  // Change process
  char *_file = strdup(file);
  proc_setname(rpc_curpid,basename(_file));
  free(_file);
  proc_setvar(rpc_curpid,var);

  // Load executable
  exe_t *exe = exe_create(file);
  if (exe!=NULL) {
    // Remove old addrspace
    size_t num_pages,i;
    void **pages = proc_mempagelist(rpc_curpid,&num_pages);
    if (pages!=NULL) {
      for (i=0;i<num_pages;i++) proc_memfree(rpc_curpid,pages[i]);
    }

    void *entrypoint = exe_load(exe,rpc_curpid);

    if (entrypoint!=NULL) {
      proc_createstack(rpc_curpid);
      proc_jump(rpc_curpid,entrypoint);
    }
    else {
      dbgmsg("failed loading executable!");
      dbgmsg("TODO: destroy process\n");
      while (1);
    }

    exe_destroy(exe);
    return 0;
  }
  else return errno;
}
