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
#include <sys/cdefs.h>
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

static void init_link(char *dir) {
  char *dest;
  asprintf(&dest,"%s%s",BOOT_MP,dir);
  symlink(dest,dir);
  free(dest);
}

static void *exe_load(pid_t pid,const char *file) {
  return elf_load(pid,file);
}

/// @todo address space of parent must be RO too! (COW)
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
        void *page = proc_memget(rpc_curpid,pages[i],NULL,&writable,&swappable,NULL);
        if (page!=NULL) proc_memmap(pid,pages[i],page,writable,swappable,1);
        else proc_memalloc(pid,pages[i],writable,swappable);
      }
    }

    // Set entrypoint of child
    proc_jump(pid,child_entry);

    proc_run(pid);
  }

  return pid;
}

/// @todo What happens when returning? caller won't request return
static void proc_exec(const char *file,int var) {
  // Change process
  char *_file = strdup(file);
  proc_stop(rpc_curpid);
  proc_setvar(rpc_curpid,var);
  proc_setname(rpc_curpid,basename(_file));

  // Remove userspace
  void *i;
  for (i=(void*)USERSPACE_ADDRESS;i<(void*)(USERSPACE_ADDRESS+USERSPACE_SIZE);i+=PAGE_SIZE) {
    proc_memfree(rpc_curpid,i);
  }

  // Load executable
  void *entrypoint = exe_load(rpc_curpid,file);
  proc_jump(rpc_curpid,entrypoint);
  proc_createstack(rpc_curpid);

  proc_run(rpc_curpid);
}

static pid_t proc_execute(const char *file,int var) {

  // Create process
  char *_file = strdup(file);
  pid_t pid = proc_create(basename(_file),getuidbypid(rpc_curpid),getgidbypid(rpc_curpid),rpc_curpid);
  proc_setvar(pid,var);

  // Load executable
  void *entrypoint = exe_load(pid,file);
  if (entrypoint==NULL) {
    proc_destroy(pid);
    return -1;
  }

  proc_jump(pid,entrypoint);
  proc_createstack(pid);

  proc_run(pid);
  return pid;
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
    else if (strcmp(INIT_PROGRAM(i),"ramdisk")==0) {
      // Initial mount of root device
      vfs_mount(ROOT_FS,ROOT_MP,ROOT_DEV,ROOT_RO);
      // create directories
      mkdir("/dev",0777);
      mkdir(BOOT_MP,0777);
      mkdir("/tmp",0777);
      mkdir("/mnt",0777);
      mkdir("/var",0777);
      mkdir("/var/log",0777);
      // create symlinks
      init_link("/bin");
      init_link("/etc");
      init_link("/usr");
    }
  }

  rpc_func(proc_fork,"i",sizeof(int));
  rpc_func(proc_exec,"$i",PATH_MAX+sizeof(int));
  rpc_func(proc_execute,"$i",PATH_MAX+sizeof(int));
  rpc_func_create("computer_shutdown",init_computer_shutdown,"",0);

  init_run(INIT2_PROGRAM);

  rpc_mainloop(-1);

  return 0;
}
