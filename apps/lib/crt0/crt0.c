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
