#include <misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static void ramdisk_link(char *dir) {
  char *dest;
  asprintf(&dest,"/boot%s",dir);
  symlink(dest,dir);
  free(dest);
}

int main() {
  // Mount ramdisk
dbgmsg("initrd: Hello!\n");
  while (vfs_mount("ramdisk","/",NULL,0)==-1) sleep(1);
dbgmsg("initrd: mounted ramdisk\n");
while (1);
  // create directories
  mkdir("/dev",0777);
  mkdir("/boot",0777);
  mkdir("/tmp",0777);
  mkdir("/mnt",0777);
  mkdir("/var",0777);
  mkdir("/var/log",0777);

  // create symlinks
  ramdisk_link("/bin");
  ramdisk_link("/etc");
  ramdisk_link("/usr");

  return 0;
}
