/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CDI_H_
#define _CDI_H_

#include <cdi/lists.h>
/// @todo remove
#include <stdio.h>

#define CDI_STANDALONE

// Debug (meinOS specific)
/*#define CDI_DEBUG(...) do { fprintf(stderr,"cdi: "); \
    fprintf(stderr,__VA_ARGS__); } while (0);*/
#define CDI_DEBUG(...)

typedef enum {
    CDI_UNKNOWN    = 0,
    CDI_NETWORK    = 1,
    CDI_STORAGE    = 2,
    CDI_GENERIC    = 3,
    CDI_VIDEO      = 4
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
