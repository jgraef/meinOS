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

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [STRING]...\n");
  fprintf(stream,"Print current work directory\n");
  fprintf(stream,"\t-h\tshow this help message\n");
  fprintf(stream,"\t-v\toutput version information and exit\n");
}

int main(int argc,char *argv[]) {
  int c;

  while ((c = getopt(argc,argv,":hv"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("pwd v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  char *cwd = getcwd(NULL,0);
  if (cwd!=NULL) {
    puts(cwd);
    free(cwd);
    return 0;
  }
  else return 1;
}
