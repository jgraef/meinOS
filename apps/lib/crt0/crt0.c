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

#include <stdlib.h>
#include <syscall.h>

int main(int argc,char *argv[]);
int _stdlib_init();
int _libmeinos_init();
int _exec_getdata(int msqid,int *argc,char ***argv);

#include <stdio.h>

void _start() {
  _stdlib_init();
  _libmeinos_init();

  // get initial data and call main
  int argc = 0;
  char **argv = NULL;
  if (_exec_getdata(syscall_call(SYSCALL_PROC_GETVAR,0),&argc,&argv)==0) exit(main(argc,argv));
  else exit(1);

  while (1);
}
