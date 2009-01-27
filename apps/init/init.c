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

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <misc.h>
#include <unistd.h>
#include <proc.h>
#include <rpc.h>
#include <libgen.h>

#include "exe_elf.h"
#include "init.conf.h"

static int sigusr1_count;

/**
 * SIGUSR1 Handler. Is called when process finishs initialzation
 *  @param sig Signal. MUST be SIGUSR1
 */
static void sigusr1_handler(int sig) {
  sigusr1_count++;
}

/**
 * Initializes Initialization ;)
 */
static void init_init() {
  signal(SIGUSR1,sigusr1_handler);
}

/**
 * Runs a program (Sends SIGCONT)
 *  @param name Name of program to run
 */
static void init_run(const char *name) {
  pid_t pid = getpidbyname(name);
  sigusr1_count = 0;
  proc_run(pid);
}

/**
 * Waits for a program to initialize
 *  @param name Name of program to wait for
 *  @return If waiting was successful or timeout occured
 */
static int init_wait(const char *name) {
  useconds_t t = 0;
  const useconds_t timeout = 10000; // 10 seconds
  while (sigusr1_count==0 && t<timeout) {
    t += 10;
    usleep(10000);
  }
  return (t<timeout);
}

typedef struct {
  int fh;
  void *data;
  enum {
    EXE_ELF
  } type;
} exe_t;

static exe_t *exe_create(const char *file) {
  exe_t *exe = malloc(sizeof(exe_t*));

  // try ELF
  exe->data = elf_create(file);
  if (exe->data!=NULL) {
    exe->type = EXE_ELF;
    return exe;
  }
  else elf_destroy((elf_t*)exe->data);

  // not an executable
  free(exe);
  return NULL;
}

static void *exe_load(exe_t *exe,pid_t pid) {
  if (exe->type==EXE_ELF) return elf_load((elf_t*)exe->data,pid);
  else return NULL;
}

static void exe_destroy(exe_t *exe) {
  if (exe->type==EXE_ELF) elf_destroy((elf_t*)exe->data);
}

static pid_t proc_fork(void *child_entry) {
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

    // Set entrypoint of child
    proc_jump(pid,child_entry);
  }

  return pid;
}

static int proc_exec(const char *file,int var) {
  // Change process
  char *_file = strdup(file);
  proc_setname(rpc_curpid,basename(_file));
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
      proc_jump(rpc_curpid,entrypoint);
      proc_createstack(rpc_curpid);
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

static void init_computer_shutdown() {
  pid_t child;
  while ((child = getchild(0))!=-1) kill(child,SIGKILL);
  exit(0);
}

int main(int argc,char *argv[]) {
  size_t i;

  init_init();
  for (i=0;INIT_PROGRAM(i);i++) {
    //dbgmsg("init: starting %s...",INIT_PROGRAM(i));
    init_run(INIT_PROGRAM(i));

    if (!init_wait(INIT_PROGRAM(i))) {
      dbgmsg("init: %s does not respond. initialization failed!\n",init_programs[i]);
      return 1;
    }
    //dbgmsg("done\n");

    if (strcmp(INIT_PROGRAM(i),"iso9660")==0) {
      // Initial mount of boot device
      vfs_mount(BOOT_FS,BOOT_MP,BOOT_DEV,BOOT_RO);
      sleep(1);
    }
  }

  rpc_func(proc_fork,"i",sizeof(int));
  rpc_func(proc_exec,"$i",PATH_MAX+sizeof(int));
  rpc_func_create("computer_shutdown",init_computer_shutdown,"",0);

  init_run(INIT2_PROGRAM);

  rpc_mainloop(-1);

  return 0;
}
