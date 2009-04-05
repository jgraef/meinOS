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
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <misc.h>

int main(int argc,char *argv[]);
void _stdlib_init();     ///< @see apps/lib/stdlibc/stdlib.c
void _libmeinos_init();  ///< @see apps/lib/libmeinos/misc.c

void _start() {
  char *backup_argv[] = { NULL };
  char **argv = backup_argv;
  int argc = 0;

  // initialize libs
  _stdlib_init();
  _libmeinos_init();

  // get process data
  int var = syscall_call(SYSCALL_PROC_GETVAR,1,getpid());
  if (var!=-1) proc_unpack_procdata(var,&argc,&argv);

  /*if (var!=-1) {
    _process_data = shmat(var,NULL,0);
    if (_process_data!=NULL) get_cmdline(_process_data,&argv,&argc,&_stdin,&_stdout,&_stderr);
  }
  else _process_data = NULL;

  // post initialize stdlibc
  _stdlib_init_post(_stdin,_stdout,_stderr);
  atexit(crt0_quit);*/

  // call main function and exit
  exit(main(argc,argv));

  while (1);
}
