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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [FILE]...\n",cmd);
  fprintf(stream,"Duplicate standard input\n");
  fprintf(stream,"\t-a\tAppend the output to the files\n");
  fprintf(stream,"\t-i\tIgnore SIGINT signal\n");
  fprintf(stream,"\t-h\tshow this help message\n");
  fprintf(stream,"\t-v\toutput version information and exit\n");
  exit(ret);
}

int main(int argc,char *argv[]) {
  int c;
  size_t num_files,i;
  int append = 0;
  int ignore = 1;
  FILE **fds;

  while ((c = getopt(argc,argv,":aihv"))!=-1) {
    switch(c) {
      case 'a':
        append = 1;
        break;
      case 'i':
        ignore = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("dirname v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  // check stdin and stdout
  if (stdin==NULL) {
    fprintf(stderr,"Can't open stdin\n");
    return 1;
  }
  if (stdout==NULL) {
    fprintf(stderr,"Can't open stdout\n");
    return 1;
  }

  // ignore SIGINT if have to
  if (ignore) signal(SIGINT,SIG_IGN);

  // open files
  num_files = argc-optind;
  fds = malloc(num_files*sizeof(FILE*));
  for (i=0;i<num_files;i++) {
    fds[i] = fopen(argv[optind+i],append?"a":"w");
    if (fds[i]==NULL) fprintf(stderr,"tee: %s: %s",argv[optind+i],strerror(errno));
  }

  // do RW
  while (!feof(stdin)) {
    char buf[1024];
    size_t size = fread(buf,1,size,stdin);
    fwrite(buf,1,size,stdout);
    for (i=0;i<num_files;i++) {
      if (fds[i]!=NULL) fwrite(buf,1,size,fds[i]);
    }
  }

  // close files
  for (i=0;i<num_files;i++) {
    if (fds[i]!=NULL) fclose(fds[i]);
  }
  free(fds);

  return 0;
}
