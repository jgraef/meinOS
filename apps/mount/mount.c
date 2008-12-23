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

#include <misc.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void usage(char *cmd,int err) {
  FILE *out = err?stderr:stdout;
  fprintf(out,"Usage: %s [-t fstype] device dir\n");
}

int main(int argc,char *argv[]) {
  char *type = NULL;
  char *dev = NULL;
  char *mountpoint = NULL;
  int c;
  int list = 0;
  int readonly = 0;

  if (argc==1) list = 1;
  else {
    while ((c = getopt(argc,argv,":ht:lVrw"))!=-1) {
      switch(c) {
        case 'h':
          usage(argv[0],0);
          break;
        case 't':
          if (type==NULL) type = strdup(optarg);
          else {
            fprintf(stderr,"Multiple FS types selected\n");
            usage(argv[0],1);
          }
          break;
        case 'l':
          list = 1;
          break;
        case 'V':
          printf("mount v0.1\n(c) Janosch Graef 2008\n");
          return 0;
          break;
        case 'r':
          readonly = 1;
          break;
        case 'w':
          readonly = 0;
          break;
        case ':':
          fprintf(stderr,"Option -%c requires an operand\n",optopt);
          usage(argv[0],1);
          break;
        case '?':
          fprintf(stderr,"Unrecognized option: -%c\n", optopt);
          usage(argv[0],1);
          break;
      }
    }
  }

  if (list==0) {
    if (optind+1<argc) {
      dev = argv[optind];
      mountpoint = argv[optind+1];
    }
    if (mountpoint==NULL) {
      fprintf(stderr,"No mountpoint given\n");
      usage(argv[0],1);
    }
    if (dev==NULL) {
      fprintf(stderr,"No device given\n");
      usage(argv[0],1);
    }
    else if (strcmp(dev,"/dev/cdrom")==0 && type==NULL) type = "iso9660";
    if (type==NULL) {
      fprintf(stderr,"No FS type given\n");
      usage(argv[0],1);
    }
    return vfs_mount(type,mountpoint,dev,readonly)==0?0:1;
  }
  else {
    printf("TODO: List\n");
    return -1;
  }
}
