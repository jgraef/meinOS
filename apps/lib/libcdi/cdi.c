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

#include <string.h>
#include <devfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <cdi/misc.h>
#include <cdi/lists.h>
#include <cdi/storage.h>
#include <cdi/fs.h>
#include <rpc.h>
#include <cdi.h>

int cdi_fs_init();

/**
 * Destroys CDI
 */
void cdi_destroy() {
  fprintf(stderr,"CDI destroy\n");
  struct cdi_driver* driver;
  while ((driver = cdi_list_pop(cdi_drivers))) driver->destroy(driver);
}

/**
 * Initializes CDI
 */
void cdi_init() {
  fprintf(stderr,"CDI init\n");
  atexit(cdi_destroy);
  cdi_drivers = cdi_list_create();
  cdi_filesystems = cdi_list_create();
  devfs_init();
  cdi_fs_init();
}

/**
 * Runs all CDI drivers
 */
void cdi_run_drivers() {
  fprintf(stderr,"CDI run drivers\n");
  struct cdi_driver* driver;
  struct cdi_device* device;
  int i, j;

  for (i=0;(driver = cdi_list_get(cdi_drivers,i));i++) {
    fprintf(stderr,"CDI init driver: %s\n",driver->name);
    for (j = 0;(device = cdi_list_get(driver->devices,j));j++) {
      device->driver = driver;
      if (driver->init_device!=NULL) driver->init_device(device);
    }
  }

  rpc_mainloop(-1);
}

/**
 * Initializes a CDI driver
 *  @param driver Driver to initialize
 */
void cdi_driver_init(struct cdi_driver* driver) {
  fprintf(stderr,"CDI driver init\n");
  driver->devices = cdi_list_create();
}

/**
 * Destroys a CDI driver
 *  @param driver Driver to destroy
 */
void cdi_driver_destroy(struct cdi_driver* driver) {
  fprintf(stderr,"CDI driver destroy: %s\n",driver->name);
  cdi_list_destroy(driver->devices);
}

/**
 * Registers a CDI driver
 *  @param driver Driver to register
 */
void cdi_driver_register(struct cdi_driver* driver) {
  fprintf(stderr,"CDI driver register: %s\n",driver->name);
  cdi_list_push(cdi_drivers,driver);
}
