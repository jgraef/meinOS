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
#include <signal.h>
#include <stdio.h>
#include <misc.h>
#include <unistd.h>
#include <sys/cdefs.h>

// Default mount parameters for root
#ifndef ROOT_FS
  #define ROOT_FS "ramdisk"
#endif
#ifndef ROOT_MP
  #define ROOT_MP "/"
#endif
#ifndef ROOT_DEV
  #define ROOT_DEV NULL
#endif
#ifndef ROOT_RO
  #define ROOT_RO 0
#endif
#if ROOT_RO==1
  #error Root must be RW!
#endif

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
  kill(pid,SIGCONT);
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

void init_link(char *dir) {
  char *dest;
  asprintf(&dest,"%s%s",BOOT_MP,dir);
  symlink(dest,dir);
  free(dest);
}

int main(int argc,char *argv[]) {
  size_t i;
  const char *init_programs[] = {
    "vfs",
    "devfs",
    "dma",
    "ata",
    "cdrom",
    "iso9660",
    "ramdisk",
    "pci",
    "psdev",
    //"console",
    //"cirrus",
    NULL
  };

  init_init();
  for (i=0;init_programs[i];i++) {
    init_run(init_programs[i]);
    if (!init_wait(init_programs[i])) {
      fprintf(stderr,"init: %s does not respond. initialization failed!\n",init_programs[i]);
      return 1;
    }
    if (strcmp(init_programs[i],"iso9660")==0) {
      // Initial mount of boot device
      printf("init: Mounting boot filesystem: mount -t %s %s %s\n",BOOT_FS,BOOT_DEV,BOOT_MP);
      vfs_mount(BOOT_FS,BOOT_MP,BOOT_DEV,BOOT_RO);
      sleep(1);
    }
    else if (strcmp(init_programs[i],"ramdisk")==0) {
      // Initial mount of root device
      printf("init: Mounting root filesystem: mount -t %s %s %s\n",ROOT_FS,ROOT_DEV,ROOT_MP);
      vfs_mount(ROOT_FS,ROOT_MP,ROOT_DEV,ROOT_RO);
      // create directories
      mkdir("/dev",0777);
      mkdir(BOOT_MP,0777);
      mkdir("/tmp",0777);
      mkdir("/mnt",0777);
      // create symlinks
      init_link("/bin");
      init_link("/etc");
      init_link("/usr");
    }
  }

  /*char *login_argv = {
    NULL
  };
  execute("/boot/usr/login",login_argv);*/

  while (1) sleep(1);

  return 0;
}
