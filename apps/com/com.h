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
#include <devfs.h>

struct com_device {
  devfs_dev_t *dev;
  uint16_t base;
};

struct com_device *devices[4];

void com_dev_init(struct com_device *dev);
size_t com_dev_recv(struct com_device *dev,const void *buf,size_t n);
size_t com_dev_send(struct com_device *dev,void *buf,size_t n);
