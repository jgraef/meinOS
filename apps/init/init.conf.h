#ifndef _INIT_CONF_H_
#define _INIT_CONF_H_

/// @todo generate this header from some config file in /etc/init.d

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
  "ata",
  "cdrom",
  "iso9660",
  "console",
  NULL
};

#define INIT_PROGRAM(n) init_programs[n]
#define INIT2_PROGRAM   "init2"

#endif
