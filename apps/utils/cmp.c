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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION] FILE1 FILE2\n",cmd);
  fprintf(stream,"Interpret carriage-control characters\n");
  fprintf(stream,"\t-l\tWrite the byte number (decimal) and the differing bytes (octal) for each difference\n");
  fprintf(stream,"\t-s\tWrite nothing for differing files; return exit status only\n");
  fprintf(stream,"\t-h\tShow this help message\n");
  fprintf(stream,"\t-v\tOutput version information and exit\n");
  exit(ret);
}

int main(int argc,char *argv[]) {
  int c;
  size_t byte = 1;
  size_t line = 1;
  FILE *file1;
  FILE *file2;
  int binary = 0;
  int quiet = 0;
  int ret = 0;

  while ((c = getopt(argc,argv,":lshv"))!=-1) {
    switch(c) {
      case 'l':
        binary = 1;
        break;
      case 's':
        quiet = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("cmp v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  if (optind<argc-1) {
    if (strcmp(argv[optind],"-")==0) file1 = stdin;
    else {
      file1 = fopen(argv[optind],"r");
      if (file1==NULL) {
        fprintf(stderr,"cmp: %s: %s\n",argv[optind],strerror(errno));
        return 1;
      }
    }

    if (strcmp(argv[optind+1],"-")==0) file2 = stdin;
    else {
      file2 = fopen(argv[optind+1],"r");
      if (file2==NULL) {
        fprintf(stderr,"cmp: %s: %s\n",argv[optind+1],strerror(errno));
        return 1;
      }
    }
  }
  else usage(argv[0],1);

  while (!feof(file1) && !feof(file2)) {
    char a = fgetc(file1);
    char b = fgetc(file2);
    if (a!=b) {
      if (!quiet) {
        if (binary) printf("%d %o %o\n",byte,a,b);
        else printf("%s %s differ: char %d, line %d\n",argv[optind],argv[optind+1],byte,line);
      }
      ret = 1;
      break;
    }
    else if (a=='\n') line++;
    byte++;
  }

  if (feof(file1) && !feof(file2)) fprintf(stderr,"cmp: EOF on %s\n",argv[optind]);
  else if (!feof(file1) && feof(file2)) fprintf(stderr,"cmp: EOF on %s\n",argv[optind+1]);

  fclose(file1);
  fclose(file2);

  return 0;
}
