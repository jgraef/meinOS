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
int _stdlib_init_pre();  ///< @see apps/lib/stdlibc/stdlib.c
int _stdlib_init_post(); ///< @see apps/lib/stdlibc/stdlib.c
int _libmeinos_init();   ///< @see apps/lib/libmeinos/misc.c


#include <stdio.h>

static void get_cmdline(struct process_data *data,char ***_argv,int *_argc) {
  char **argv = NULL;
  int argc;
  size_t i = 0;

  for (argc=0;data->cmdline[i]!=0;argc++) {
    argv = realloc(argv,(argc+1)*sizeof(char*));
    argv[argc] = data->cmdline+i;
    i += strlen(data->cmdline+i)+1;
  }

  *_argv = argv;
  *_argc = argc;
}

void _start() {
  int var,argc,ret;
  char **argv;

  // initialize libs
  _stdlib_init_pre();
  _libmeinos_init();

  // get process data
  var = syscall_call(SYSCALL_PROC_GETVAR,1,getpid());
  if (var!=-1) {
    _process_data = shmat(var,NULL,0);
    if (_process_data!=NULL) get_cmdline(_process_data,&argv,&argc);
  }

  // post initialize stdlibc
  _stdlib_init_post();

  // call main function
  ret = main(argc,argv);

  // remove process data
  shmdt(_process_data);
  shmctl(var,IPC_RMID,NULL);

  // exit
  exit(ret);
  while (1);
}
