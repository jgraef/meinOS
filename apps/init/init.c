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
#include <rpc.h>

#include "init.h"

/*static void init_computer_shutdown() {
  pid_t child;
  while ((child = getchild(0))!=-1) kill(child,SIGKILL);
  exit(0);
}*/

int main(int argc,char *argv[]) {
  // Initialize standard init RPC functions
  rpc_func(proc_fork,"i",sizeof(int));
  rpc_func(proc_exec,"$i",PATH_MAX+sizeof(int));
  //rpc_func_create("computer_shutdown",init_computer_shutdown,"",0);

  // Start GRUB modules
  pid_t *grub_modules = init_get_grub_modules();
  init_sort_grub_modules(grub_modules);
  if (init_run_grub_modules(grub_modules)==-1) return 1;

  // Initialize ProcFS
  procfs_init(grub_modules);
  procfs_run();

  return 0;
}
