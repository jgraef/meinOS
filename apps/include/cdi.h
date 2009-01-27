/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _CDI_H_
#define _CDI_H_

#include <cdi/lists.h>

#define CDI_STANDALONE

// Debug (meinOS specific)
/*#define CDI_DEBUG(...) do { fprintf(stderr,"cdi: "); \
    fprintf(stderr,__VA_ARGS__); } while (0);*/
#define CDI_DEBUG(...)

typedef enum {
    CDI_UNKNOWN    = 0,
    CDI_NETWORK    = 1,
    CDI_STORAGE    = 2,
    CDI_SCSI       = 3,
    CDI_GENERIC    = 4, /* not standard */
    CDI_VIDEO      = 5  /* not standard */
} cdi_device_type_t;

struct cdi_driver;
struct cdi_device {
  cdi_device_type_t type;
  const char* name;
  struct cdi_driver* driver;
};

struct cdi_driver {
  cdi_device_type_t   type;
  const char*         name;
  cdi_list_t          devices;
  void (*init_device)(struct cdi_device* device);
  void (*remove_device)(struct cdi_device* device);
  void (*destroy)(struct cdi_driver *driver);
};

/// @note meinOS specific
cdi_list_t cdi_drivers;
cdi_list_t cdi_filesystems;

void cdi_init();
void cdi_run_drivers();
void cdi_driver_init(struct cdi_driver* driver);
void cdi_driver_destroy(struct cdi_driver* driver);
void cdi_driver_register(struct cdi_driver* driver);

#endif
