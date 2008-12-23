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

#include <stdio.h>
#include <llist.h>
#include <rpc.h>
#include <syscall.h>

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [STRING]...\n",cmd);
  fprintf(stream,"report interprocess communication facilities status\n");
  fprintf(stream,"\t-q\tWrite information about active message queues\n");
  fprintf(stream,"\t-m\tWrite information about active shared memory segments\n");
  fprintf(stream,"\t-s\tWrite information about active semaphore sets\n");
  fprintf(stream,"\t-r\tWrite information about active RPC functions\n");
  fprintf(stream,"\t-h\tShow this help message\n");
  fprintf(stream,"\t-v\tOutput version information and exit\n");
  exit(ret);
}

void display_msg() {
}
void display_shm() {
}
void display_sem() {
}

#ifdef WITH_RPC
void display_rpc() {
  llist_t list = rpc_list();

  printf("------- Remote Procedure Call: Functions ---------\n");
  printf("Name\tSynopsis\tParameter Buffer Size\n");
  while (func = llist_pop(list)) {
    size_t paramsz;
    char synopsis[RPC_SYNOPSIS_MAXLEN];

    if (syscall_call(SYSCALL_RPC_GETINFO,8,-1,name,0,0,&paramsz,synopsis,RPC_SYNOPSIS_MAXLEN,0)==0) {
      printf("%s\t%s\t%d\n",name,synopsis,paramsz);
    }
  }
}
#endif

int main(int argc,char *argv[]) {
  int c;
  int show_msg = 0;
  int show_shm = 0;
  int show_sem = 0;
  int show_rpc = 0;
  int show_time = 0;
  int show_pid = 0;
  int show_creator = 0;
  int show_limits = 0;
  int show_outstanding = 0;

  while ((c = getopt(argc,argv,":qmsabcoptrhv"))!=-1) {
    switch(c) {
      case 'q':
        show_msg = 1;
        break;
      case 'm':
        show_shm = 1;
        break;
      case 's':
        show_sem = 1;
        break;
#ifdef WITH_RPC
      case 'r':
        show_rpc = 1;
        break;
#endif
      case 'a':
        show_time = 1;
        show_pid = 1;
        show_creator = 1;
        show_limits = 1;
        show_outstanding = 1;
        break;
      case 'b':
        show_limits = 1;
        break;
      case 'c':
        show_creator = 1;
        break;
      case 'o':
        show_outstanding = 1;
        break;
      case 'p':
        show_pid = 1;
        break;
      case 't':
        show_time = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("yes v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  if (!show_msg && !show_shm && !show_sem && !show_rpc) {
    show_msg = 1;
    show_shm = 1;
    show_sem = 1;
  }

  if (!show_time && !show_pid && !show_creator && !show_limits && !show_outstanding) {
    show_time = 1;
    show_pid = 1;
    show_creator = 1;
    show_limits = 1;
    show_outstanding = 1;
  }

  if (show_msg) display_msg();
  if (show_shm) display_shm();
  if (show_sem) display_sem();
#ifdef WITH_RPC
  if (show_rpc) display_rpc();
#endif

  return 0;
}
