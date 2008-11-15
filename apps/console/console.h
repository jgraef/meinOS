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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdio.h>
#include <sys/types.h>
#include <wchar.h>
#include <devfs.h>

// keyboard.c
int init_keyboard();
ssize_t onread(devfs_dev_t *dev,void *buffer,size_t count,off_t offset);

// screen.c
int init_screen();
ssize_t onwrite(devfs_dev_t *dev,void *buffer,size_t count,off_t offset);

#endif
