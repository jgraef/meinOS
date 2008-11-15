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

#include <stdint.h>
#include <stddef.h>

#include "cdi.h"
#include "cdi/generic.h"

#include "com.h"

struct cdi_generic_driver driver;
char *driver_name = "com";

static void com_driver_init();
static void com_driver_destroy(struct cdi_driver *driver);

#ifdef CDI_STANDALONE
int main()
#else
int init_com()
#endif
{
  cdi_init();
  com_driver_init();
  cdi_generic_driver_register(&driver);

#ifdef CDI_STANDALONE
  cdi_run_drivers();
#endif

  return 0;
}

static void com_create_function(struct cdi_generic_driver *drv),int (*func_ptr)(struct cdi_generic_device *dev,void *data,size_t datasz,void *ret,size_t retsz)) {
  struct cdi_generic_function *func = malloc(sizeof(struct cdi_generic_function));
  func->func = func_ptr;
  cdi_list_push(dev->functions,func);
}

void com_driver_init() {
  cdi_generic_driver(&driver);

  driver.drv.name = driver_name;

  driver.drv.destroy = com_driver_destroy;
  driver.drv.init_device = com_init_device;
  driver.drv.remove_device = com_remove_device;

  /// @todo Read from Bios Data Area
  uint16_t io_ports[] = {0x3F8,0x2F8,0x3E8,0x2E8};
  size_t i;
  for (i=0;i<4;i++) {
    if (io_ports[i]!=0) {
      struct com_device *dev = malloc(sizeof(struct com_device));
      dev->dev.stream = malloc(sizeof(struct cdi_generic_stream));
      dev->dev.dev.name = malloc(5);
      sprintf(&(dev->dev.dev.name),"com%d",i);
      dev->dev.stream->objsz = 1;
      dev->dev.stream->read = com_read;
      dev->dev.stream->write = com_write;
      dev->dev.functions = cdi_list_create();
      com_create_function((struct cdi_generic_driver*)dev,com_setbaud);
      com_create_function((struct cdi_generic_driver*)dev,com_setparity);
      com_create_function((struct cdi_generic_driver*)dev,com_setbytelen);
      com_create_function((struct cdi_generic_driver*)dev,com_setnstpbits);
      dev->base_ioport = io_ports[i];
      cdi_list_push(driver->devices,dev);
    }
  }
}