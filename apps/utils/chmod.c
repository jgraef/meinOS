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

#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

static void usage(char *cmd,int ret) {
  FILE *stream = ret==0?stdout:stderr;
  fprintf(stream,"Usage: %s [OPTION]... MODE FILE...\n",cmd);
  fprintf(stream,"Changes mode of a file\n");
  fprintf(stream,"\t-R\tRecurse in directories\n");
  fprintf(stream,"\t-h\tShow this help message\n");
  fprintf(stream,"\t-v\tOutput version information and exit\n");
  exit(ret);
}

static void do_chmod(const char *path,mode_t mode,int recursive) {
  if (chmod(path,mode)==-1) fprintf(stderr,"chown: %s: %s\n",path,strerror(errno));

  if (recursive) {
    struct stat stbuf;
    stat(path,&stbuf);
    if (S_ISDIR(stbuf.st_mode)) {
      DIR *dir = opendir(path);
      if (dir!=NULL) {
        while (1) {
          struct dirent *dirent = readdir(dir);
          if (dirent==NULL) break;

          char *newpath;
          asprintf(&newpath,"%s/%s");
          do_chmod(newpath,mode,recursive);
          free(newpath);
        }
        closedir(dir);
      }
      else fprintf(stderr,"opendir: %s: %s\n",path,strerror(errno));
    }
  }
}

static mode_t str2mode(const char *str) {
  if (str[0]=='0') return (mode_t)strtoul(str,NULL,8);
  else if (strlen(str)==9) return (str[0]=='r'?S_IRUSR:0)|(str[1]=='w'?S_IWUSR:0)|(str[2]=='x'?S_IXUSR:0)
                                 |(str[3]=='r'?S_IRGRP:0)|(str[4]=='w'?S_IWGRP:0)|(str[5]=='x'?S_IXGRP:0)
                                 |(str[6]=='r'?S_IROTH:0)|(str[7]=='w'?S_IWOTH:0)|(str[8]=='x'?S_IXOTH:0);
  else return -1;
}

int main(int argc,char *argv[]) {
  int c;
  int recursive = 0;
  size_t i;
  char *mode_str;
  char **files;

  while ((c = getopt(argc,argv,":Rhv"))!=-1) {
    switch(c) {
      case 'R':
        recursive = 1;
        break;
      case 'h':
        usage(argv[0],0);
        break;
      case 'v':
        printf("chmod v0.1\n(c) 2008 Janosch Graef\n");
        return 0;
        break;
      case '?':
        fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        usage(argv[0],1);
        break;
    }
  }

  // get parameters
  if (optind!=argc) mode_str = argv[optind];
  else {
    fprintf(stderr,"No mode given\n");
    usage(argv[0],1);
  }
  optind++;
  if (optind!=argc) files = argv+optind;
  else {
    fprintf(stderr,"No files given\n");
    usage(argv[0],1);
  }

  mode_t mode = str2mode(mode_str);
  if (mode==-1) {
    fprintf(stderr,"Invalid mode\n");
    usage(argv[0],1);
  }
  for (i=0;files[i];i++) do_chmod(files[i],mode,recursive);

  return 0;
}
