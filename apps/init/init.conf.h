#ifndef _INIT_CONF_H_
#define _INIT_CONF_H_

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

// Drivers to initialize
static const char *init_programs[] = {
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
#define INIT_PROGRAM(n) init_programs[n]
#define INIT2_PROGRAM   "init2"

#endif
