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
#include <stddef.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

static void usage(char *prog,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [STRING]...\n",prog);
  fprintf(stream,"Echo the STRING(s) to standard output\n");
  fprintf(stream,"\t-A\tSame as -vET\n");
  fprintf(stream,"\t-b\tNumber not empty lines\n");
  fprintf(stream,"\t-e\tSame as -vE\n");
  fprintf(stream,"\t-E\tPrint $ after each line\n");
  fprintf(stream,"\t-n\tNumber all lines\n");
  fprintf(stream,"\t-s\tSqueeze continuing blank lines\n");
  fprintf(stream,"\t-t\tSame as -vT\n");
  fprintf(stream,"\t-T\tOutput tabulator as ^I\n");
  fprintf(stream,"\t-u\tignored\n");
  fprintf(stream,"\t-v\tUse ^- and M-notation, except for linefeed and tabulator\n");

  fprintf(stream,"\t-h\tdisplay this help and exit\n");
  fprintf(stream,"\t-V\toutput version information and exit\n");
  exit(ret);
}

// flags
static int non_print,display_ends,display_tabs,squeeze_blank;
static enum { NO, NONBLANK, ALL } number;

// current count of new lines
static int blanklines_cur,blanklines,lines;

void escape_string(char chr) {
  if (chr=='\a') puts("^G");
  else if (chr=='\b') puts("^H");
  else if (chr=='\f') puts("^L");
  else if (chr=='\r') puts("^M");
  else if (chr=='\v') puts("^K");
  else putchar(chr);
}

static inline void print_line(char nextchr) {
  if (number!=NO && nextchr!=0 && (number==ALL || nextchr!='\n' || nextchr!='\r')) {
    printf(" % 4d  ",lines);
  }
}

static void cat(FILE *fd) {
  char buf[BUFSIZ+1];
  buf[BUFSIZ] = 0; // for print_line()

  print_line(buf[0]);

  while (!feof(fd)) {
    size_t count = fread(buf,1,BUFSIZ,fd);
    size_t i;
    for (i=0;i<count;i++) {
      // display non printable
      if (!isgraph(buf[i]) && buf[i]!='\t' && buf[i]!='\n' && non_print) {
        escape_string(buf[i]);
      }
      // display new line
      // count blank lines
      // count lines
      // squeeze blank lines
      else if (buf[i]=='\n') {
        if (display_ends) putchar('$');
        if (blanklines_cur<2) putchar('\n');
        else blanklines++;
        blanklines_cur++;
        print_line(buf[i+1]);
        lines++;
      }
      // display tab
      else if (buf[i]=='\t' && display_tabs) {
        puts("^I");
      }
      else putchar(buf[i]);
      // reset current cound of blank lines
      if (buf[i]!='\n' && buf[i]!='\r') blanklines_cur = 0;
    }
  }
}

int main(int argc,char *argv[]) {
  int c,i;
  non_print = 0;
  display_ends = 0;
  display_tabs = 0;
  squeeze_blank = 0;
  number = NO;
  blanklines_cur = 0;
  blanklines = 0;
  lines = 0;

  while ((c = getopt(argc,argv,":AbeEnstTuvhV"))!=-1) {
    switch(c) {
      case 'A':
        non_print = 1;
        display_ends = 1;
        display_tabs = 1;
        break;
      case 'b':
        number = NONBLANK;
        break;
      case 'e':
        non_print = 1;
        display_ends = 1;
        break;
      case 'E':
        display_ends = 1;
        break;
      case 'n':
        number = ALL;
        break;
      case 's':
        squeeze_blank = 1;
        break;
      case 't':
        non_print = 1;
        display_tabs = 1;
        break;
      case 'T':
        display_tabs = 1;
        break;
      case 'u':
        // ignored
        break;
      case 'v':
        non_print = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'V':
        printf("cat v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  for (i=optind;i<argc;i++) {
    FILE *fd = strcmp(argv[i],"-")==0?stdin:fopen(argv[i],"r");
    if (fd!=NULL) {
      cat(fd);
      fclose(fd);
    }
    else perror("fopen");
  }

  return 0;
}
