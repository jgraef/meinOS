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

#include <devfs.h>
#include <unistd.h>
#include <stdio.h>

#include "console.h"

/**
 * Initialize console driver
 */
int main() {
  devfs_dev_t *console;

  init_screen();
  init_keyboard();

  devfs_init();
  console = devfs_createdev("console");
  devfs_onread(console,onread);
  devfs_onwrite(console,onwrite);

  devfs_mainloop();
  return 0;
}
