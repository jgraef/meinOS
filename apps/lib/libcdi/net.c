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

#include <sys/types.h>
#include <string.h>
#include <cdi.h>
#include <cdi/net.h>

void cdi_net_driver_init(struct cdi_net_driver* driver) {
  cdi_driver_init((struct cdi_driver*)driver);
}

void cdi_net_driver_destroy(struct cdi_net_driver* driver) {
  cdi_driver_destroy((struct cdi_driver*)driver);
  memset(&driver,0,sizeof(driver));
}

void cdi_net_driver_register(struct cdi_net_driver* driver) {
  cdi_driver_register((struct cdi_driver*)driver);
}

void cdi_net_device_init(struct cdi_net_device* device) {

}

void cdi_net_receive(struct cdi_net_device* device,void *buffer,size_t size) {

}
