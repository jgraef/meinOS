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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

void usage(char *prog,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [STRING]...\n");
  fprintf(stream,"Echo the STRING(s) to standard output\n");
  fprintf(stream,"\t-n\tdo not output the trailing newline\n");
  fprintf(stream,"\t-e\tenable interpretation of backslash escapes\n");
  fprintf(stream,"\t-E\tdisable interpretation of backslash escapes (default)\n");
  fprintf(stream,"\t-h\tdisplay this help and exit\n");
  fprintf(stream,"\t-v\toutput version information and exit\n");
}

/**
 * Converts a string with an octal number to an integer
 *  @param string String holding octal number
 *  @return String as integer
 */
int octal2num(char *str) {
  char buf[4];
  memcpy(buf,str,3);
  buf[3] = 0;
  int num = strtoul(buf,NULL,8);
  return num;
}

char *escape_string(char *str,int *newline) {
  char *new = malloc(strlen(str)+1);
  size_t i,j;
  for (i=0,j=0;str[i];i++,j++) {
    if (str[i]=='\\' && str[i+1]!=0) {     // escape code
      i++;
      if (str[i]=='\\') new[j] = '\\';     // backslash
      else if (str[i]=='a') new[j] = '\a'; // alert
      else if (str[i]=='b') new[j] = '\b'; // backspace
      else if (str[i]=='c') *newline = 0;  // supress trailing newline
      else if (str[i]=='f') new[j] = '\f'; // form feed
      else if (str[i]=='n') new[j] = '\n'; // new line
      else if (str[i]=='r') new[j] = '\r'; // carriage return
      else if (str[i]=='t') new[j] = '\t'; // horizontal tab
      else if (str[i]=='v') new[j] = '\v'; // vertical tab
      else if (str[i]=='0' && str[i+1]!=0 && str[i+2]!=0 && str[i+3]!=0) {
                                           // character in octal
        new[j] = octal2num(str+i+1);
      }
    }
    else new[j] = str[i];
  }
  new[j] = 0;
  return new;
}

int main(int argc,char *argv[]) {
  int c;
  int newline = 1;
  int escape = 0;

  while ((c = getopt(argc,argv,":neEhv"))!=-1) {
    switch(c) {
      case 'n':
        newline = 0;
        break;
      case 'e':
        escape = 1;
        break;
      case 'E':
        escape = 0;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("echo v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  for (c=optind;c<argc;c++) {
    char *string;
    if (escape) string = escape_string(argv[c],&newline);
    else string = argv[c];

    printf("%s",string);
    if (c<argc-1) putchar(' ');

    if (escape) free(string);
  }

  if (newline) putchar('\n');

  return 0;
}
