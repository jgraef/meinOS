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

#include <pwd.h>
#include <unistd.h>
#include <stdio.h>

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... [STRING]...\n");
  fprintf(stream,"Print the real and effective UIDs and GIDs\n");
  fprintf(stream,"\t-g\tPrint only GID\n");
  fprintf(stream,"\t-n\tDisplay name instead of ID\n");
  fprintf(stream,"\t-r\tUse real ID instead of effective one\n");
  fprintf(stream,"\t-u\tPrint only UID\n");
  fprintf(stream,"\t-h\tshow this help message\n");
  fprintf(stream,"\t-v\toutput version information and exit\n");
}

int main(int argc,char *argv[]) {
  int c;
  int show_user = 1;
  int show_group = 1;
  int use_real = 0;
  int show_name = 0;

  while ((c = getopt(argc,argv,":gnruhv"))!=-1) {
    switch(c) {
      case 'g':
        show_user = 0;
        break;
      case 'n':
        show_name = 1;
        break;
      case 'r':
        use_real = 1;
        break;
      case 'u':
        show_group = 0;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("id v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  struct passwd *pwd = getpwuid(use_real?getuid():geteuid());
  if (pwd==NULL) {
    perror("getpwuid");
    return 1;
  }

  /// @todo Output in right format
  /// http://www.opengroup.org/onlinepubs/009695399/utilities/id.html
  if (show_user) {
    if (show_name) puts(pwd->pw_name);
    else printf("%d\n",pwd->pw_uid);
  }
  if (show_group) {
    if (show_name) printf("TODO: %s:%d\n",__FILE__,__LINE__);
    else printf("%d\n",pwd->pw_gid);
  }

  return 0;
}
