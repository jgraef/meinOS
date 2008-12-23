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
  fprintf(stream,"Usage: %s PATH [FILE]...\n",cmd);
  fprintf(stream,"Interpret carriage-control characters\n");
  fprintf(stream,"\t-h\tshow this help message\n");
  fprintf(stream,"\t-v\toutput version information and exit\n");
  exit(ret);
}

static void asa(FILE *in,FILE *out) {
  size_t num_lines = 0;
  size_t i;
  char **in_lines = NULL;
  char **out_lines;

  // read file
  while (!feof(in)) {
    in_lines = realloc(in_lines,(num_lines+1)*sizeof(char*));
    in_lines[num_lines] = malloc(1024);
    fgets(in_lines[num_lines],1024,in);
    in_lines[num_lines] = realloc(in_lines[num_lines],strlen(in_lines[num_lines])+1);
    num_lines++;
  }

  // interpret carriage-control characters
  out_lines = malloc(num_lines*sizeof(char*));
  for (i=0;i<num_lines;i++) {
    if (in_lines[i][0]==' ') out_lines[i] = in_lines[i]+1;
    else if (in_lines[i][0]=='0') {
      out_lines[i] = in_lines[i];
      out_lines[i][0] = '\n';
    }
    else if (in_lines[i][0]=='1') {
      out_lines[i] = in_lines[i];
      out_lines[i][0] = '\f';
    }
    else if (in_lines[i][0]=='+') {
      out_lines[i] = in_lines[i]+1;
      if (i>0) out_lines[i-1][strlen(out_lines[i-1])-1] = '\r';
    }
    else out_lines[i] = in_lines[i];
  }

  // print (and free)
  for (i=0;i<num_lines;i++) {
    fputs(out_lines[i],out);
    free(in_lines[i]);
  }
  free(in_lines);
  free(out_lines);
}

int main(int argc,char *argv[]) {
  int c,i;

  while ((c = getopt(argc,argv,":hv"))!=-1) {
    switch(c) {
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("asa v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  if (optind==argc) asa(stdin,stdout);
  else {
    for (i=optind;i<argc;i++) {
      FILE *fd = fopen(argv[optind],"r");
      if (fd!=NULL) {
        asa(fd,stdout);
        fclose(fd);
      }
      else fprintf(stderr,"asa: %s: %s\n",argv[optind],strerror(errno));
    }
  }

  return 0;
}
