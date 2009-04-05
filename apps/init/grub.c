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
#include <stdlib.h>
#include <unistd.h>
#include <misc.h>
#include <signal.h>

// Default mount parameters for boot device
#ifndef BOOT_FS
  #define BOOT_FS "iso9660"
#endif
#ifndef BOOT_MP
  #define BOOT_MP "/boot"
#endif
#ifndef BOOT_DEV
  #define BOOT_DEV "/dev/cdrom0"
#endif
#ifndef BOOT_RO
  #define BOOT_RO 1
#endif

static int sigusr1_count = 0;

/**
 * SIGUSR1 Handler. Is called when process finishs initialzation
 *  @param sig Signal. MUST be SIGUSR1
 */
static void sigusr1_handler(int sig) {
  sigusr1_count++;
}

/**
 * Waits for a program to initialize
 *  @return If waiting was successful or timeout occured
 */
static int init_wait_grub_module(void) {
  useconds_t t = 0;
  const useconds_t timeout = 10000; // 10 seconds
  while (sigusr1_count==0 && t<timeout) {
    t += 10;
    usleep(10000);
  }
  sigusr1_count = 0;
  return (t<timeout)?0:-1;
}

static int compare(const void *ve1,const void *ve2) {
  const pid_t *e1 = ve1;
  const pid_t *e2 = ve2;

  return *e1-*e2;
}
void init_sort_grub_modules(pid_t *modules) {
  size_t n;

  for (n=0;modules[n]!=0;n++);

  qsort(modules,n,sizeof(pid_t),compare);
}

/**
 * Gets grub modules
 *  @return List of grub modules
 */
pid_t *init_get_grub_modules() {
  size_t i;
  pid_t child;
  pid_t *children = NULL;

  for (i=0;(child = getchild(i))!=-1;i++) {
    children = realloc(children,(i+1)*sizeof(pid_t));
    children[i] = child;
  }
  children = realloc(children,(i+1)*sizeof(pid_t));
  children[i] = 0;

  return children;
}

/**
 * Runs grub modules
 *  @param modules List of GRUB modules
 */
int init_run_grub_modules(pid_t *modules) {
  size_t i;

  signal(SIGUSR1,sigusr1_handler);

  for (i=0;modules[i]!=0;i++) {
    char *name = getname(modules[i]);

    proc_run(modules[i]);

    if (init_wait_grub_module()==-1) {
      dbgmsg("init: Process %s does not respond. initialization failed!\n",name);
      return -1;
    }

    if (strcmp(name,"iso9660")==0) {
      // Initial mount of boot device
      vfs_mount(BOOT_FS,BOOT_MP,BOOT_DEV,BOOT_RO);
    }

    free(name);
  }

  return 0;
}
