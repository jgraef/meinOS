/*
 * iso9660 - An iso9660 CDI driver with Rockridge support
 *
 * Copyright (C) 2008 Janosch Gräf
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>

#include "cdi/fs.h"

#include "iso9660_cdi.h"
#include "iso9660def.h"

struct iso9660_driver {
  struct cdi_fs_driver drv;
};

static struct iso9660_driver iso9660_driver;
static const char* driver_name = "iso9660";

static int iso9660_driver_init(struct iso9660_driver *driver);
static void iso9660_driver_destroy(struct cdi_driver* driver);

#ifdef CDI_STANDALONE
int main()
#else
int init_iso9660()
#endif
{
    cdi_init();

    if (iso9660_driver_init(&iso9660_driver)!=0) return -1;
    cdi_fs_driver_register((struct cdi_fs_driver*)&iso9660_driver);

#ifdef CDI_STANDALONE
    cdi_run_drivers();
#endif

    return 0;
}

/**
 * Initializes the data structures for the iso9660 driver
 */
static int iso9660_driver_init(struct iso9660_driver *driver) {
    // Konstruktor der Vaterklasse
    cdi_fs_driver_init((struct cdi_fs_driver*)driver);

    // Namen setzen
    driver->drv.drv.name = driver_name;
    driver->drv.fs_init = iso9660_fs_init;
    driver->drv.fs_destroy = iso9660_fs_destroy;
    driver->drv.drv.destroy = iso9660_driver_destroy;
    return 0;
}

/**
 * Deinitialize the data structures for the iso9660 driver
 */
static void iso9660_driver_destroy(struct cdi_driver* driver)
{
    cdi_fs_driver_destroy((struct cdi_fs_driver*)driver);
}

/**
 * If DEBUG is definded, it outputs the debug message onto the stream
 * defined with DEBUG
 *  @param fmt Format (see printf)
 *  @param ... Parameters
 *  @return Amount of output characters
 */
int debug(const char *fmt,...) {
#ifdef DEBUG
  va_list args;
  va_start(args,fmt);
  int ret = vfprintf(DEBUG,fmt,args);
  va_end(args);
  return ret;
#else
  return 0;
#endif
}
