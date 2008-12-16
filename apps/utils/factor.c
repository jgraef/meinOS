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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "factor_wheel.h"

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [NUMBER]...\n");
  fprintf(stream,"Print prime factors of each number\n");
  fprintf(stream,"\t-h\tshow this help message\n");
  fprintf(stream,"\t-v\toutput version information and exit\n");
}

static void factor(char *str) {
  unsigned int n,n0,q;
  unsigned int d = 2;
  unsigned char const *w = wheel_tab;

  errno = 0;
  if ((n0 = strtoul(str,NULL,10))==0 && errno!=0) {
    fprintf(stderr,"factor: %s: %s\n",str,strerror(errno));
    return;
  }

  printf("%d:\n",n0);
  n = n0;
  do {
    q = n/d;
    while (n==q*d) {
      printf(" %d",d);
      n = q;
      q = n / d;
    }
    d += *(w++);
    if (w==WHEEL_END) w = WHEEL_START;
  }
  while (d<=q);

  if (n!=1 || n0==1) printf(" %d",n);

  putchar('\n');
}

int main(int argc,char *argv[]) {
  int c;

  while ((c = getopt(argc,argv,":hv"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("factor v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  if (optind<argc) {
    int i;
    for (i=optind;i<argc;i++) factor(argv[i]);
  }
  else {
    if (stdin==NULL) {
      fprintf(stderr,"factor: can't use stdin\n");
      return 1;
    }

    char buf[1024];
    while (!feof(stdin)) {
      char *line = fgets(buf,1024,stdin);
      if (line!=NULL) factor(line);
    }
  }

  return 0;
}
